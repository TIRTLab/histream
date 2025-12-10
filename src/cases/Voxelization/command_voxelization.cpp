//
// Created by Fan Tengyuan on 9/12/2025.
//
#include "command_voxelization.h"

bool Command_voxelization::create(std::shared_ptr<VoxelizationIO> &modelio)
{
    auto &m_device = modelio->m_device;

    // 创建 Fence 用于 CPU 等待 GPU 完成
    // 初始状态为 Unsignaled
    VkFenceCreateInfo fci = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    vkCreateFence(m_device, &fci, nullptr, &modelio->m_fence);

    return true;
}

bool Command_voxelization::run(std::shared_ptr<VoxelizationIO>& modelio)
{
    // 1. 准备 CommandBuffer
    // 使用 IO 中初始化的 CommandPool 创建一个临时的 CommandBuffer
    VkCommandBuffer cmd = modelio->m_genCmdBuf.createCommandBuffer();

    // 计算最大维度，用于视口设置
    uint32_t maxDim = std::max({modelio->setting.gridSize.x,
                                modelio->setting.gridSize.y,
                                modelio->setting.gridSize.z});

    // 2. 开始 Render Pass
    // 这是一个 "Dummy" Render Pass，没有颜色附件，只为了提供光栅化上下文
    VkRenderPassBeginInfo rpInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpInfo.renderPass        = modelio->m_renderPass;
    rpInfo.framebuffer       = modelio->m_framebuffer;
    rpInfo.renderArea.offset = {0, 0};
    rpInfo.renderArea.extent = {maxDim, maxDim}; // 覆盖最大范围
    rpInfo.clearValueCount   = 0;
    rpInfo.pClearValues      = nullptr;

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    // 3. 绑定管线和描述符
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, modelio->m_pipeline);

    // 绑定 3D 纹理 (Set 0)
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, modelio->m_pipelineLayout,
                            0, 1, &modelio->m_descSet, 0, nullptr);

    // 4. 设置动态状态 (Viewport & Scissor)
    VkViewport viewport = {0.0f, 0.0f, (float)maxDim, (float)maxDim, 0.0f, 1.0f};
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor = {{0, 0}, {maxDim, maxDim}};
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    // 5. 绘制循环 (遍历所有 Instance)
    auto& instanceList = modelio->m_instanceio->instances;
    auto& meshBuffers  = modelio->m_meshio->m_bufferMeshes;

    for (const auto& inst : instanceList) {
        int meshId = inst.meshId;

        // 安全检查：防止 meshId 越界
        if (meshId < 0 || meshId >= meshBuffers.size()) continue;

        auto& mesh = meshBuffers[meshId];

        // 5.1 绑定顶点和索引缓冲
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmd, 0, 1, &mesh.vertexBuffer.buffer, offsets);
        vkCmdBindIndexBuffer(cmd, mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        // 5.2 准备 Push Constants
        PushConstantVoxel pc;
        pc.modelMatrix = inst.object2worldMatrix; // 物体的世界变换矩阵
        // ObjectID: 通常 0 代表空气，所以我们将物体 ID 设为 meshId + 1 或 loop index + 1
        // 这里假设使用 meshId 作为材质 ID
        pc.objectID    = meshId + 1;

        // 传入 Grid Size 用于几何着色器归一化坐标
        pc.volumeInfo  = glm::vec4(maxDim, 0, 0, 0);

        // 5.3 推送常量
        vkCmdPushConstants(cmd, modelio->m_pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0, sizeof(pc), &pc);

        // 5.4 绘制调用
        vkCmdDrawIndexed(cmd, mesh.nbIndices, 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(cmd);

    // 6. 添加内存屏障 (可选但推荐)
    // 确保 Shader 对 Image 的写入全部完成，并且对后续操作可见
    VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT; // 等待写入完成
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT; // 后续可能是读取或拷贝
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = modelio->m_voxelTexture.image;
    barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    vkCmdPipelineBarrier(cmd,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, // 源阶段：片元着色器
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT, // 目标阶段：计算或传输
        0, 0, nullptr, 0, nullptr, 1, &barrier);

    // 结束录制
    vkEndCommandBuffer(cmd);

    // 7. 提交并等待
    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    // 提交到队列
    vkQueueSubmit(modelio->m_queue, 1, &submitInfo, modelio->m_fence);

    // 等待 CPU 侧同步
    vkWaitForFences(modelio->m_device, 1, &modelio->m_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(modelio->m_device, 1, &modelio->m_fence);

    // 清理临时 CommandBuffer (如果 CommandPool 是 transient 的可以不手动 free，依赖 pool reset)
    // 这里简单起见直接让 pool 管理，或者显式 free
    // vkFreeCommandBuffers(modelio->m_device, modelio->m_genCmdBuf, 1, &cmd);
    return false;
}

void Command_voxelization::destroy(std::shared_ptr<VoxelizationIO>& modelio)
{
    vkDestroyFence(modelio->m_device, modelio->m_fence, nullptr);
}

