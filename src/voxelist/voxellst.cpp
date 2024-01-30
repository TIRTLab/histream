//
// Created by admin on 2024/1/26.
//

#include "voxellst.h"



bool Voxellst::setup( AppSetting &appsetting, std::shared_ptr<VoxellstIO> &voxellstio){


    voxellstio->m_device = appsetting.m_context.m_device;
    voxellstio->m_physicalDevice = appsetting.m_context.m_physicalDevice;
    voxellstio->m_instance = appsetting.m_context.m_instance;
    voxellstio->m_queues = appsetting.m_queues;
    voxellstio->m_queue =  voxellstio->m_queues[eGCT].queue;
    voxellstio->m_queueIndex = voxellstio->m_queues[eGCT].familyIndex;
    //    m_instance = appSetting.m_context.m_instance;
//    m_device = appSetting.m_context.m_device;
//    m_physicalDevice = appSetting.m_context.m_physicalDevice;
//    m_queues = appSetting.m_queues;


//    VkCommandPoolCreateInfo poolCreateInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
//    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//    vkCreateCommandPool(voxellstio->m_device, &poolCreateInfo, nullptr, &voxellstio->m_cmdPool);

    voxellstio->m_genCmdBuf.init(voxellstio->m_device,voxellstio->m_queueIndex);


//    VkPipelineCacheCreateInfo pipelineCacheInfo{ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
//    vkCreatePipelineCache(voxellstio->m_device, &pipelineCacheInfo, nullptr, &voxellstio->m_pipelineCache);

    voxellstio->m_pAlloc  = std::make_shared<Allocator>();
    voxellstio->m_pAlloc->init(voxellstio->m_instance, voxellstio->m_device, voxellstio->m_physicalDevice);
    voxellstio->m_debug.setup(voxellstio->m_device);


    return true;

}




bool Voxellst::upload(std::shared_ptr<FileIO> &fileio,std::shared_ptr<VoxellstIO> &voxellstio){

    // auto & fileio = voxellstio->m_fileio;
    // auto & meshio = voxellstio->m_meshio;


    m_pCompo->createCompProperty(fileio, voxellstio);
    m_pScene->createPrimScene(fileio,voxellstio);
    m_pGeometry->createGeometry(fileio,voxellstio);
//    defineOPO(voxellstio);
    uploadSetting(fileio, voxellstio);

    return true;
}

bool Voxellst::uploadSetting(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &voxellstio) {

    voxellstio->n_wave = fileio->m_pRaytracingXml->sensorxml.waves.size();
    voxellstio->n_angle = fileio->m_pRaytracingXml->sensorxml.viewAngles.size();
    voxellstio->isTemperature =  fileio->m_pRaytracingXml->sensorxml.isTemperature;
    voxellstio->isDisplay = fileio->m_pRaytracingXml->sensorxml.isDisplay;
    voxellstio->isAlbedo = fileio->m_pRaytracingXml->sensorxml.isAlbedo;
    voxellstio->isImage = fileio->m_pRaytracingXml->sensorxml.isImage;
    voxellstio->resolution = fileio->m_pRaytracingXml->sensorxml.resolution;
    voxellstio->maxDepth = fileio->m_pRaytracingXml->settingxml.maxDepth;
    voxellstio->n_sample = fileio->m_pRaytracingXml->settingxml.n_sample;

    return true;
}

bool Voxellst::updateSetting(std::shared_ptr<VoxellstIO> &voxellstio){

    // auto &opo = voxellstio->m_opo;

    // auto &sceneio = voxellstio->m_sceneio;
    voxellstio->setting.resolution = voxellstio->resolution;
    voxellstio->setting.n_wave = voxellstio->n_wave;
    voxellstio->setting.isTemperature = voxellstio->isTemperature;
    voxellstio->setting.isDisplay = voxellstio->isDisplay;
    voxellstio->setting.maxDepth = voxellstio->maxDepth;
    voxellstio->setting.n_sample = voxellstio->n_sample;

    return true;
    //voxellstio->setting.maxDepth = fileio->m_pXmlInput.
}