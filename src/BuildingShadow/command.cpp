//
// Created by ShIP on 2024/12/2.
//
#include "command.h"

bool Command::create(std::shared_ptr<BuildingShadowIO> &modelio){
    // for the firest init
    VkFenceCreateInfo fci = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    vkCreateFence(modelio->m_device, &fci, nullptr, &modelio->m_fence);
    //    vkWaitForFences(modelio->m_device, 1, &(modelio->m_fence), VK_TRUE, UINT64_MAX);
    vkResetFences(modelio->m_device, 1,  &(modelio->m_fence));

    // Here 4 means number of pipelines + 1
    modelio->m_semaphores.resize(modelio->n_pipeline+1);
    std::generate_n( modelio->m_semaphores.begin(), modelio->n_pipeline+1,
                     [&]
                     {
                         VkSemaphoreCreateInfo sci = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
                         VkSemaphore semaphore;
                         vkCreateSemaphore( modelio->m_device, &sci, nullptr, &semaphore);
                         return semaphore;
                     });

    return false;
}

bool Command::run(std::shared_ptr<BuildingShadowIO>& modelio)
{
    modelio->m_currentSemaphore = 1;

    glm::ivec3 voxelSize1D = glm::ivec3((modelio->n_facet + (GROUP_SIZEX - 1)) / GROUP_SIZEX, 1, 1);
    auto & descSet = modelio->m_descSet;
    // auto & setting = modelio->setting;

    submit(modelio, BuildingShadowStage::gap, voxelSize1D, std::nullopt, std::nullopt);
    waitFence(modelio);

    return true;
}

void Command::submit(std::shared_ptr<BuildingShadowIO>& modelio, BuildingShadowStage stage, glm::ivec3 dispatchSize,
    const std::optional<std::vector<VkSemaphore>>& inSemaphore, const std::optional<VkSemaphore>& outSemaphore)
{
    auto & descSet = modelio->m_descSet;
    auto & setting = modelio->setting;
    auto & pipelineLayout = modelio->m_pipelineLayout;
    auto & pipeline = modelio->m_pipelines[stage];


    auto & m_currentSemaphore = modelio->m_currentSemaphore;
    // Preparing for the compute shader
    VkCommandBuffer cmdBuf = modelio->m_genCmdBuf.createCommandBuffer();

    // Dispatching the shader only for the other;
    recordCommandBuffer(cmdBuf, descSet, pipelineLayout, pipeline, setting );
    vkCmdDispatch(cmdBuf, dispatchSize.x, dispatchSize.y, dispatchSize.z);

    auto & semaphores = modelio->m_semaphores;
    // new semaphores
    std::array<VkPipelineStageFlags, 1> waitStages{VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT};
    VkSubmitInfo submitInfoCompute{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfoCompute.commandBufferCount = 1;
    submitInfoCompute.pCommandBuffers = &cmdBuf;

    vkEndCommandBuffer(cmdBuf);
    vkQueueSubmit( modelio->m_queue, 1, &submitInfoCompute,  modelio->m_fence);
    m_currentSemaphore++;
}

void Command::recordCommandBuffer(VkCommandBuffer cmdBuf, VkDescriptorSet descSet, VkPipelineLayout pipelineLayout,
    VkPipeline pipeline, BuildingShadowSetting setting)
{
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0,
                            static_cast<uint32_t>(1), &descSet, 0, nullptr);
    // Sending the push constant information
    vkCmdPushConstants(cmdBuf, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(VoxelLstSetting), &setting);

}

void Command::waitFence(std::shared_ptr<BuildingShadowIO>& modelio)
{
    vkWaitForFences(modelio->m_device, 1, &(modelio->m_fence), VK_TRUE, UINT64_MAX);
    vkResetFences(modelio->m_device, 1,  &(modelio->m_fence));
}

void Command::destroy(std::shared_ptr<BuildingShadowIO>& modelio)
{
    vkDestroyFence(modelio->m_device, modelio->m_fence, nullptr);
    // vkFreeCommandBuffers(m_device, m_cmdPool, 1, &m_commandBuffers[i]);

    for (auto& semaphore : modelio->m_semaphores)
    {
        vkDestroySemaphore(modelio->m_device, semaphore, nullptr);
    }
}

