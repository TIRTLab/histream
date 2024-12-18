//
// Created by ShIP on 2024/12/2.
//
#include <io.h>
#include "pipeline.h"

bool Pipeline::createPipeline(std::shared_ptr<BuildingShadowIO>& modelio)
{
    auto & m_device = modelio->m_device;
    auto & m_pipelineLayout = modelio->m_pipelineLayout;
    auto & m_pipelines = modelio->m_pipelines;
    auto & m_sbtWrapper = modelio->m_sbtWrapper;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & useDeferred = modelio->useDeferred;
    auto & useSBTWrapper = modelio->useSBTWrapper;
    auto & m_pAlloc = modelio->m_pAlloc;
    auto & m_debug = modelio->m_debug;


    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

    VkPushConstantRange pushConstant{VK_SHADER_STAGE_COMPUTE_BIT,
                                     0, sizeof(VoxelLstSetting)};
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(1);
    pipelineLayoutCreateInfo.pSetLayouts = &m_descSetLayout;
    vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);


    std::string baseDirectory = modelio->defineDir;
    std::map<BuildingShadowStage, std::string> VoxellstPaths{
            {BuildingShadowStage::gap,         baseDirectory + "/shader/buildingShadow/sunlitArea.comp.spv"}};


    if (_access(VoxellstPaths[BuildingShadowStage::gap].c_str(), 0) == -1)
    {
        std::string error = "Error: no shader file (.spv) found in " + baseDirectory + "\n";
        LOGI(error.c_str());
    }

    for (const auto &stage : magic_enum::enum_values<BuildingShadowStage>())
    {
        //int stageInt = (int)stage;
        VkComputePipelineCreateInfo computePipelineCreateInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        computePipelineCreateInfo.layout = m_pipelineLayout;
        computePipelineCreateInfo.stage =
                nvvk::createShaderStageInfo(m_device, nvh::loadFile(VoxellstPaths[stage], true), VK_SHADER_STAGE_COMPUTE_BIT);

        vkCreateComputePipelines(m_device, {}, 1, &computePipelineCreateInfo, nullptr, &m_pipelines[stage]);

        m_debug.setObjectName(m_pipelines[stage], "VoxelLST");
        vkDestroyShaderModule(m_device, computePipelineCreateInfo.stage.module, nullptr);
    }

    return true;
}

void Pipeline::destroy(std::shared_ptr<BuildingShadowIO>& modelio)
{
    for (auto &stage : magic_enum::enum_values<BuildingShadowStage>()) {
        vkDestroyPipeline(modelio->m_device, modelio->m_pipelines[stage], nullptr);
        modelio->m_pipelines[stage] = VkPipeline();
    }
    vkDestroyPipelineLayout(modelio->m_device, modelio->m_pipelineLayout, nullptr);
    modelio->m_pipelineLayout = VkPipelineLayout();
}
