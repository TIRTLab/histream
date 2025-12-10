//
// Created by Fan Tengyuan on 9/12/2025.
//
#include "pipeline_voxelization.h"

// ----------------------------------------------------------------
// Push Constant 定义
// 必须严格与 Shader (voxelizer.vert/geom/frag) 中的定义内存对齐
// ----------------------------------------------------------------
// struct PushConstantVoxel {
//     glm::mat4 modelMatrix;   // [64 bytes] 物体到世界的变换矩阵
//     uint32_t  objectID;      // [4 bytes]  物体 ID (用于写入纹理的值)
//     float     padding[3];    // [12 bytes] 填充，保证后续 vec4 对齐
//     glm::vec4 volumeInfo;    // [16 bytes] .xyz = gridSize (网格分辨率), .w = unused
// };

bool Pipeline_voxelization::create(std::shared_ptr<VoxelizationIO> &modelio) {
    auto &m_device = modelio->m_device;
    auto &setting  = modelio->setting;

    // ----------------------------------------------------------------
    // 1. 创建 Render Pass (Dummy / 空)
    // ----------------------------------------------------------------
    // 体素化是通过 imageAtomicMax 写入 Storage Image 的，不经过传统的 ROP 混合
    // 因此不需要任何 Color Attachment 或 Depth Attachment
    VkRenderPassCreateInfo rpInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    rpInfo.attachmentCount = 0;
    rpInfo.pAttachments    = nullptr;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 0; // 无颜色输出
    subpass.pColorAttachments    = nullptr;
    subpass.pDepthStencilAttachment = nullptr;

    rpInfo.subpassCount = 1;
    rpInfo.pSubpasses   = &subpass;

    // 添加子流程依赖，确保外部内存可见性 (虽然主要依赖 Barrier，但这是好习惯)
    VkSubpassDependency dependency = {};
    dependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass      = 0;
    dependency.srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependency.dstStageMask    = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependency.srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
    dependency.dstAccessMask   = VK_ACCESS_SHADER_WRITE_BIT;
    rpInfo.dependencyCount     = 1;
    rpInfo.pDependencies       = &dependency;

    vkCreateRenderPass(m_device, &rpInfo, nullptr, &modelio->m_renderPass);

    // ----------------------------------------------------------------
    // 2. 创建 Framebuffer (Dummy / 空)
    // ----------------------------------------------------------------
    // 即使没有附件，光栅化器也需要知道渲染区域的大小。
    // 我们将其设置为体素网格的最大维度，确保任何角度的投影都不会被裁剪。
    uint32_t maxDim = std::max({setting.gridSize.x, setting.gridSize.y, setting.gridSize.z});

    VkFramebufferCreateInfo fbInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    fbInfo.renderPass      = modelio->m_renderPass;
    fbInfo.attachmentCount = 0;
    fbInfo.pAttachments    = nullptr;
    fbInfo.width           = maxDim;
    fbInfo.height          = maxDim;
    fbInfo.layers          = 1;

    vkCreateFramebuffer(m_device, &fbInfo, nullptr, &modelio->m_framebuffer);

    // ----------------------------------------------------------------
    // 3. Pipeline Layout (资源绑定布局)
    // ----------------------------------------------------------------
    // 定义 Push Constant 范围
    VkPushConstantRange pushRange = {};
    // 顶点、几何、片元着色器都需要访问这些常量
    pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushRange.offset     = 0;
    pushRange.size       = sizeof(PushConstantVoxel);

    VkPipelineLayoutCreateInfo layoutInfo = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    layoutInfo.setLayoutCount         = 1;
    layoutInfo.pSetLayouts            = &modelio->m_descSetLayout; // 引用 Descriptor 创建好的 Layout
    layoutInfo.pushConstantRangeCount = 1;
    layoutInfo.pPushConstantRanges    = &pushRange;

    vkCreatePipelineLayout(m_device, &layoutInfo, nullptr, &modelio->m_pipelineLayout);

    // ----------------------------------------------------------------
    // 4. 加载 Shaders (Vertex + Geometry + Fragment)
    // ----------------------------------------------------------------
    std::string shaderPath = modelio->definedDir + "/shader/cases/Voxelization/";

    // 使用 nvvk 辅助函数加载 shader module
    // 必须确保 .spv 文件存在于该路径下
    VkPipelineShaderStageCreateInfo stages[3];
    stages[0] = nvvk::createShaderStageInfo(m_device, nvh::loadFile(shaderPath + "voxelizer.vert.spv", true, {}), VK_SHADER_STAGE_VERTEX_BIT);
    stages[1] = nvvk::createShaderStageInfo(m_device, nvh::loadFile(shaderPath + "voxelizer.geom.spv", true, {}), VK_SHADER_STAGE_GEOMETRY_BIT);
    stages[2] = nvvk::createShaderStageInfo(m_device, nvh::loadFile(shaderPath + "voxelizer.frag.spv", true, {}), VK_SHADER_STAGE_FRAGMENT_BIT);

    // ----------------------------------------------------------------
    // 5. Pipeline State: Vertex Input
    // ----------------------------------------------------------------
    // 复用 MeshIO 中的 Vertex 结构。我们只需要 Position 数据。
    // 假设 struct Vertex { vec3 pos; vec3 n; vec2 t; ... };
    VkVertexInputBindingDescription bindingDesc = {0, sizeof(VertexAttribute), VK_VERTEX_INPUT_RATE_VERTEX};

    // Attribute 0: Position (vec3)
    VkVertexInputAttributeDescription attribDesc = {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexAttribute, pos)};

    VkPipelineVertexInputStateCreateInfo vertexInput = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInput.vertexBindingDescriptionCount   = 1;
    vertexInput.pVertexBindingDescriptions      = &bindingDesc;
    vertexInput.vertexAttributeDescriptionCount = 1;
    vertexInput.pVertexAttributeDescriptions    = &attribDesc;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // ----------------------------------------------------------------
    // 6. Pipeline State: Viewport (Dynamic)
    // ----------------------------------------------------------------
    VkPipelineViewportStateCreateInfo viewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    // ----------------------------------------------------------------
    // 7. Pipeline State: Rasterization (关键：保守光栅化)
    // ----------------------------------------------------------------
    VkPipelineRasterizationStateCreateInfo rasterizer = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode                = VK_CULL_MODE_NONE; // 【重要】必须关闭剔除，确保双面都生成体素
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.lineWidth               = 1.0f;

    // 配置保守光栅化扩展
    // 作用：只要三角形触碰到像素的任何部分，就生成片元。防止薄物体漏掉。
    VkPipelineRasterizationConservativeStateCreateInfoEXT conservativeState =
        {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT};
    conservativeState.conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT;
    conservativeState.extraPrimitiveOverestimationSize = 0.0f;

    // 将扩展结构体链入 pNext
    rasterizer.pNext = &conservativeState;

    // ----------------------------------------------------------------
    // 8. Pipeline State: Multisample & Depth & Blend
    // ----------------------------------------------------------------
    VkPipelineMultisampleStateCreateInfo multisampling = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampling.sampleShadingEnable   = VK_FALSE;
    multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;

    // 关闭深度测试 (我们手动写入 3D 纹理，不需要 Z-Buffer)
    VkPipelineDepthStencilStateCreateInfo depthStencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depthStencil.depthTestEnable       = VK_FALSE;
    depthStencil.depthWriteEnable      = VK_FALSE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_ALWAYS;
    depthStencil.stencilTestEnable     = VK_FALSE;

    // 关闭颜色混合 (没有 Color Attachment)
    VkPipelineColorBlendStateCreateInfo colorBlending = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.attachmentCount   = 0;
    colorBlending.pAttachments      = nullptr;

    // ----------------------------------------------------------------
    // 9. Pipeline State: Dynamic States
    // ----------------------------------------------------------------
    // 允许在 CommandBuffer 中动态调整视口大小
    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicStateInfo.pDynamicStates    = dynamicStates.data();

    // ----------------------------------------------------------------
    // 10. 创建 Graphics Pipeline
    // ----------------------------------------------------------------
    VkGraphicsPipelineCreateInfo pipelineInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipelineInfo.stageCount          = 3; // Vert, Geom, Frag
    pipelineInfo.pStages             = stages;
    pipelineInfo.pVertexInputState   = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = &depthStencil;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicStateInfo;
    pipelineInfo.layout              = modelio->m_pipelineLayout;
    pipelineInfo.renderPass          = modelio->m_renderPass;
    pipelineInfo.subpass             = 0;
    pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

    VkResult result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &modelio->m_pipeline);

    if (result != VK_SUCCESS) {
        // 简单的错误处理，实际工程中建议添加 LOGE
        return false;
    }

    // 清理 Shader Modules (Pipeline 创建后就不需要了)
    for (auto& s : stages) {
        vkDestroyShaderModule(m_device, s.module, nullptr);
    }
    return false;
}

void Pipeline_voxelization::destroy(std::shared_ptr<VoxelizationIO>& modelio)
{
    vkDestroyPipeline(modelio->m_device, modelio->m_pipeline, nullptr);
    vkDestroyPipelineLayout(modelio->m_device, modelio->m_pipelineLayout, nullptr);
    vkDestroyRenderPass(modelio->m_device, modelio->m_renderPass, nullptr);
    vkDestroyFramebuffer(modelio->m_device, modelio->m_framebuffer, nullptr);

    modelio->m_pipeline = VK_NULL_HANDLE;
    modelio->m_pipelineLayout = VK_NULL_HANDLE;
    modelio->m_renderPass = VK_NULL_HANDLE;
    modelio->m_framebuffer = VK_NULL_HANDLE;
}

