//
// Created by admin on 2024/1/26.
//
#include <io.h>
#include "pipeline.h"




void Pipeline::createShaderBindingTable(std::shared_ptr<VoxellstIO> &modelio)
{




}

bool Pipeline::createPipeline(std::shared_ptr<VoxellstIO> &modelio)
{


    auto & m_device = modelio->m_device;
    auto & m_pipelineLayout = modelio->m_pipelineLayout;
    auto & m_pipelines = modelio->m_pipelines;
    auto & m_sbtWrapper = modelio->m_sbtWrapper;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & useDeferred = modelio->useDeferred;
    auto & useSBTWrapper = modelio->useSBTWrapper;
    auto & m_pAlloc = modelio->m_pAlloc;
    auto & m_rtSBTBuffer = modelio->m_rtSBTBuffer;
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


    std::string baseDirectory = modelio->shaderDir;
    std::map<VoxellstStage, std::string> VoxellstPaths{
            {VoxellstStage::gap, baseDirectory + "/shader/Voxelrad/voxelrad_solar.comp.spv"},
            {VoxellstStage::directVNIR, baseDirectory + "/shader/Voxelrad/voxelrad_direct_VNIR.comp.spv"},
            {VoxellstStage::directTIR, baseDirectory + "/shader/Voxelrad/voxelrad_direct_TIR.comp.spv"},
            {VoxellstStage::diffuseVNIR, baseDirectory + "/shader/Voxelrad/voxelrad_diffuse_VNIR_single.comp.spv"},
            {VoxellstStage::diffuseTIR, baseDirectory + "/shader/Voxelrad/voxelrad_diffuse_TIR_single.comp.spv"},
            {VoxellstStage::budget, baseDirectory + "/shader/EB/budget.comp.spv"},
            {VoxellstStage::evapo, baseDirectory + "/shader/ET/evapo.comp.spv"},
            {VoxellstStage::updateTp, baseDirectory + "/shader/ET/updateTp.comp.spv"},
            {VoxellstStage::updateL, baseDirectory + "/shader/EB/updateL.comp.spv"},
            {VoxellstStage::aero, baseDirectory + "/shader/aero/aeresist.comp.spv"},
            {VoxellstStage::bio, baseDirectory + "/shader/bio/biochemical.comp.spv"},
            {VoxellstStage::out, baseDirectory + "/shader/Voxelrad/voxelrad_image.comp.spv"}};


    if (_access(VoxellstPaths[VoxellstStage::gap].c_str(), 0) == -1)
    {
        std::string error = "Error: no shader file (.spv) found in " + baseDirectory + "\n";
        LOGI(error.c_str());
    }

    for (const auto &stage : magic_enum::enum_values<VoxellstStage>())
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


bool Pipeline::createRTPipeline(std::shared_ptr<VoxellstIO> &modelio)
{


    auto & m_device = modelio->m_device;
    auto & m_pipelineLayout = modelio->m_pipelineLayout_rt;
    auto & m_pipelines = modelio->m_pipelines_rt;
    auto & m_sbtWrapper = modelio->m_sbtWrapper;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & useDeferred = modelio->useDeferred;
    auto & useSBTWrapper = modelio->useSBTWrapper;
    auto & m_pAlloc = modelio->m_pAlloc;
    auto & m_rtSBTBuffer = modelio->m_rtSBTBuffer;
    auto & m_debug = modelio->m_debug;

    std::string baseDirectory = modelio->shaderDir;
    std::map<VoxelRadStage, std::string> VoxeltracingPaths{
            {VoxelRadStage::gap, baseDirectory + "/shader/Voxelrad/voxelrad_solar.comp.spv"},
            {VoxelRadStage::directVNIR, baseDirectory + "/shader/Voxelrad/voxelrad_direct_VNIR.comp.spv"},
            {VoxelRadStage::directTIR, baseDirectory + "/shader/Voxelrad/voxelrad_direct_TIR.comp.spv"},
            {VoxelRadStage::diffuseVNIR, baseDirectory + "/shader/Voxelrad/voxelrad_diffuse_VNIR_single.comp.spv"},
            {VoxelRadStage::diffuseTIR, baseDirectory + "/shader/Voxelrad/voxelrad_diffuse_TIR_single.comp.spv"},
            };


    if (_access(VoxeltracingPaths[VoxelRadStage::gap].c_str(), 0) == -1)
    {
        std::string error = "Error: no shader file (.spv) found in " + baseDirectory + "\n";
        LOGI(error.c_str());
    }

    for (const auto &stage : magic_enum::enum_values<VoxelRadStage>())
    {
        int stageInt = (int)stage;
        VkComputePipelineCreateInfo computePipelineCreateInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        computePipelineCreateInfo.layout = m_pipelineLayout;
        computePipelineCreateInfo.stage =
                nvvk::createShaderStageInfo(m_device, nvh::loadFile(VoxeltracingPaths[stage], true), VK_SHADER_STAGE_COMPUTE_BIT);

        vkCreateComputePipelines(m_device, {}, 1, &computePipelineCreateInfo, nullptr, &m_pipelines[stageInt]);

        m_debug.setObjectName(m_pipelines[stageInt], "VoxelLST");
        vkDestroyShaderModule(m_device, computePipelineCreateInfo.stage.module, nullptr);
    }

    return true;
}


bool Pipeline::createETPipeline(std::shared_ptr<VoxellstIO> &modelio)
{


    auto & m_device = modelio->m_device;
    auto & m_pipelineLayout = modelio->m_pipelineLayout_et;
    auto & m_pipelines = modelio->m_pipelines_et;
    auto & m_sbtWrapper = modelio->m_sbtWrapper;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & useDeferred = modelio->useDeferred;
    auto & useSBTWrapper = modelio->useSBTWrapper;
    auto & m_debug = modelio->m_debug;

    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    VkPushConstantRange pushConstant{VK_SHADER_STAGE_COMPUTE_BIT,
                                     0, sizeof(ETSetting)};

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(1);
    pipelineLayoutCreateInfo.pSetLayouts = &m_descSetLayout;
    vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);

    std::string baseDirectory = modelio->shaderDir;
    std::map<ETStage, std::string> ETShaderPaths{
            {ETStage::evapo, baseDirectory + "/shader/ET/evapo.comp.spv"},
            {ETStage::updateTp, baseDirectory + "/shader/ET/updateTp.comp.spv"}};

    for (const auto &stage : magic_enum::enum_values<ETStage>())
    {
        int stageInt = (int)stage;
        VkComputePipelineCreateInfo computePipelineCreateInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        computePipelineCreateInfo.layout = m_pipelineLayout;
        computePipelineCreateInfo.stage =
                nvvk::createShaderStageInfo(m_device, nvh::loadFile(ETShaderPaths[stage], true), VK_SHADER_STAGE_COMPUTE_BIT);

        vkCreateComputePipelines(m_device, {}, 1, &computePipelineCreateInfo, nullptr, &m_pipelines[stageInt]);
        m_debug.setObjectName(m_pipelines[stageInt], "Evapo");
        vkDestroyShaderModule(m_device, computePipelineCreateInfo.stage.module, nullptr);
    }

    return true;
}


bool Pipeline::createEBPipeline(std::shared_ptr<VoxellstIO> &modelio)
{


    auto & m_device = modelio->m_device;
    auto & m_pipelineLayout = modelio->m_pipelineLayout_et;
    auto & m_pipelines = modelio->m_pipelines_eb;
    auto & m_sbtWrapper = modelio->m_sbtWrapper;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & useDeferred = modelio->useDeferred;
    auto & useSBTWrapper = modelio->useSBTWrapper;
    auto & m_debug = modelio->m_debug;

    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

    VkPushConstantRange pushConstant{VK_SHADER_STAGE_COMPUTE_BIT,
                                     0, sizeof(EBSetting)};
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(1);
    pipelineLayoutCreateInfo.pSetLayouts = &m_descSetLayout;
    vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);

    std::string baseDirectory = modelio->shaderDir;
    std::map<EBStage, std::string> EBShaderPaths{
            {EBStage::budget, baseDirectory + "/shader/EB/budget.comp.spv"},
            {EBStage::updateL, baseDirectory + "/shader/EB/updateL.comp.spv"}};

    for (const auto &stage : magic_enum::enum_values<EBStage>())
    {
        int stageInt = (int)stage;
        VkComputePipelineCreateInfo computePipelineCreateInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        computePipelineCreateInfo.layout = m_pipelineLayout;
        computePipelineCreateInfo.stage =
                nvvk::createShaderStageInfo(m_device, nvh::loadFile(EBShaderPaths[stage], true), VK_SHADER_STAGE_COMPUTE_BIT);

        vkCreateComputePipelines(m_device, {}, 1, &computePipelineCreateInfo, nullptr, &m_pipelines[stageInt]);
        m_debug.setObjectName(m_pipelines[stageInt], "Budget");
        vkDestroyShaderModule(m_device, computePipelineCreateInfo.stage.module, nullptr);
    }

    return true;
}


bool Pipeline::createAeroPipeline(std::shared_ptr<VoxellstIO> &modelio)
{

    auto & m_device = modelio->m_device;
    auto & m_pipelineLayout = modelio->m_pipelineLayout_et;
    auto & m_pipeline = modelio->m_pipeline_aero;
    auto & m_sbtWrapper = modelio->m_sbtWrapper;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & useDeferred = modelio->useDeferred;
    auto & useSBTWrapper = modelio->useSBTWrapper;
    auto & m_debug = modelio->m_debug;

    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    VkPushConstantRange pushConstant{VK_SHADER_STAGE_COMPUTE_BIT,
                                     0, sizeof(AeroSetting)};

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(1);
    pipelineLayoutCreateInfo.pSetLayouts = &m_descSetLayout;
    vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);

    std::string baseDirectory = modelio->shaderDir;
    const std::string aeroShader = baseDirectory + "/shader/aero/aeresist.comp.spv";

    VkComputePipelineCreateInfo computePipelineCreateInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    computePipelineCreateInfo.layout = m_pipelineLayout;
    computePipelineCreateInfo.stage =
            nvvk::createShaderStageInfo(m_device, nvh::loadFile(aeroShader, true), VK_SHADER_STAGE_COMPUTE_BIT);

    vkCreateComputePipelines(m_device, {}, 1, &computePipelineCreateInfo, nullptr, &m_pipeline);

    m_debug.setObjectName(m_pipeline, "Aero");
    vkDestroyShaderModule(m_device, computePipelineCreateInfo.stage.module, nullptr);

    return true;
}


bool Pipeline::createBioPipeline(std::shared_ptr<VoxellstIO> &modelio)
{


    auto & m_device = modelio->m_device;
    auto & m_pipelineLayout = modelio->m_pipelineLayout_bio;
    auto & m_pipeline = modelio->m_pipeline_bio;
    auto & m_sbtWrapper = modelio->m_sbtWrapper;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & useDeferred = modelio->useDeferred;
    auto & useSBTWrapper = modelio->useSBTWrapper;
    auto & m_debug = modelio->m_debug;

    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    VkPushConstantRange pushConstant{VK_SHADER_STAGE_COMPUTE_BIT,
                                     0, sizeof(BioSetting)};

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstant;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(1);
    pipelineLayoutCreateInfo.pSetLayouts = &m_descSetLayout;
    vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);

    std::string baseDirectory =  modelio->shaderDir;
    const std::string bioShader = baseDirectory + "/shader/bio/biochemical.comp.spv";

    VkComputePipelineCreateInfo computePipelineCreateInfo{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    computePipelineCreateInfo.layout = m_pipelineLayout;
    computePipelineCreateInfo.stage =
            nvvk::createShaderStageInfo(m_device, nvh::loadFile(bioShader, true), VK_SHADER_STAGE_COMPUTE_BIT);

    vkCreateComputePipelines(m_device, {}, 1, &computePipelineCreateInfo, nullptr, &m_pipeline);

    m_debug.setObjectName(m_pipeline, "Bio");
    vkDestroyShaderModule(m_device, computePipelineCreateInfo.stage.module, nullptr);

    return true;
}



void Pipeline::destroy(std::shared_ptr<VoxellstIO> &modelio) {

    // modelio->m_sbtWrapper.destroy();
   // modelio->m_pAlloc->destroy((modelio->m_rtSBTBuffer));

    vkDestroyPipeline(modelio->m_device, modelio->m_pipeline_aero, nullptr);
    vkDestroyPipelineLayout(modelio->m_device, modelio->m_pipelineLayout_aero, nullptr);
    vkDestroyPipeline(modelio->m_device, modelio->m_pipeline_bio, nullptr);
    vkDestroyPipelineLayout(modelio->m_device, modelio->m_pipelineLayout_bio, nullptr);

    for (auto &stage : magic_enum::enum_values<EBStage>())
    {
        int stageInt = (int)stage;
        vkDestroyPipeline(modelio->m_device, modelio->m_pipelines_eb[stageInt], nullptr);
        modelio->m_pipelines_eb[stageInt] =  VkPipeline();
    }
    modelio->m_pipelineLayout_eb = VkPipelineLayout();


    for (auto &stage : magic_enum::enum_values<ETStage>())
    {
        int stageInt = (int)stage;
        vkDestroyPipeline(modelio->m_device, modelio->m_pipelines_et[stageInt], nullptr);
        modelio->m_pipelines_et[stageInt] =  VkPipeline();
    }
    modelio->m_pipelineLayout_et = VkPipelineLayout();


    for (auto &stage : magic_enum::enum_values<VoxelRadStage>())
    {
        int stageInt = (int)stage;
        vkDestroyPipeline(modelio->m_device, modelio->m_pipelines_rt[stageInt], nullptr);
        modelio->m_pipelines_rt[stageInt] =  VkPipeline();
    }
    modelio->m_pipelineLayout_rt = VkPipelineLayout();



}






