//
// Created by Fan Tengyuan on 9/12/2025.
//
#include "pipeline.h"

bool Pipeline::create(std::shared_ptr<VoxelizationIO> &modelio) {
    auto & m_device = modelio->m_device;
    auto & m_pipelineLayout = modelio->m_pipelineLayout;
    auto & m_pipeline = modelio->m_pipeline;
//    auto & m_sbtWrapper = modelio->m_sbtWrapper;
    auto & m_descSetLayout = modelio->m_descSetLayout;


    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    VkPushConstantRange pushConstantRange{VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR,
                                          0, sizeof(RayRTSetting) };
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(1);
    pipelineLayoutCreateInfo.pSetLayouts = &m_descSetLayout;
    vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// stages in pipeline
    /// </summary>
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    std::array<VkPipelineShaderStageCreateInfo, eShaderGroupCount> stages{};
    VkPipelineShaderStageCreateInfo stage{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    stage.pName = "main";

    std::string baseDirectory = modelio->definedDir;
    std::string offlineRaytracingRgenShader = baseDirectory + "/shader/raytracing/offline.rgen.spv";
    std::string rayTracingRchitShader = baseDirectory + "/shader/raytracing/closestHit.rchit.spv";
    std::string rayTracingRmissShader = baseDirectory + "/shader/raytracing/rmiss.rmiss.spv";
    std::string rayTracingShadowRmissShader = baseDirectory + "/shader/raytracing/shadow.rmiss.spv";

    if (access(offlineRaytracingRgenShader.c_str(), 0) == -1)
    {
        std::string error = "Error: no shader file (.spv) found in " + baseDirectory + "\n";
        LOGI(error.c_str());
    }

    // raygen
    vk::ShaderModule rgen = nvvk::createShaderModule(m_device, nvh::loadFile(offlineRaytracingRgenShader, true));
    stage.module = rgen;
    stage.stage = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
    stages[eRaygen] = stage;

    // emiss
    stage.module = nvvk::createShaderModule(m_device, nvh::loadFile(rayTracingRmissShader, true));
    stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
    stages[eMiss] = stage;

    // emiss2;
    /// the second miss shader is invoked when a shadow ray misses the geometry.
    /// it simply indicates that no occlusion has been found
    stage.module = nvvk::createShaderModule(m_device, nvh::loadFile(rayTracingShadowRmissShader, true));
    stage.stage = VK_SHADER_STAGE_MISS_BIT_KHR;
    stages[eMiss2] = stage;

    // Hit group - Closest hit
    stage.module = nvvk::createShaderModule(m_device, nvh::loadFile(rayTracingRchitShader, true));
    stage.stage = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    stages[eClosestHit] = stage;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// groups
    /// </summary>
    std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups;
    VkRayTracingShaderGroupCreateInfoKHR group{ VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR };
    group.anyHitShader = VK_SHADER_UNUSED_KHR;
    group.closestHitShader = VK_SHADER_UNUSED_KHR;
    group.generalShader = VK_SHADER_UNUSED_KHR;
    group.intersectionShader = VK_SHADER_UNUSED_KHR;
    // raygen
    group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    group.generalShader = eRaygen;
    groups.push_back(group);
    // Miss
    group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    group.generalShader = eMiss;
    groups.push_back(group);
    // shadow miss
    group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    group.generalShader = eMiss2;
    groups.push_back(group);
    // closest hit shader
    group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    group.generalShader = VK_SHADER_UNUSED_KHR;
    group.closestHitShader = eClosestHit;
    groups.push_back(group);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// pipeline
    /// </summary>
    VkRayTracingPipelineCreateInfoKHR rayPipelineInfo{ VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR };
    rayPipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
    rayPipelineInfo.pStages = stages.data();
    rayPipelineInfo.groupCount = static_cast<uint32_t>(groups.size());
    rayPipelineInfo.pGroups = groups.data();
    rayPipelineInfo.maxPipelineRayRecursionDepth = 2; /// ray depth, no use
    rayPipelineInfo.layout = m_pipelineLayout;

    // deferred operations: spreading work fot a single command across multiple CPU cores.
    VkResult result;
    VkDeferredOperationKHR deferredOp{ VK_NULL_HANDLE };


    vkCreateRayTracingPipelinesKHR(m_device, deferredOp, {}, 1, &rayPipelineInfo, nullptr, &m_pipeline);

//    m_sbtWrapper.create(m_pipeline, rayPipelineInfo);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for (auto& s : stages)
    {
        vkDestroyShaderModule(m_device, s.module, nullptr);
    }
    return false;
}

