//
// Created by admin on 2024/1/25.
//

#include "engine.h"



void Engine::init(Mode mode)
{
    appSetting.init();

    if(mode == Mode::eRaytracing) {
        m_pRaytracing = std::make_shared<Raytracing>();
        m_pRaytracingio = std::make_shared<RaytracingIO>();

    }else if(mode == Mode::eVoxelLST) {
        m_pVoxellst = std::make_shared<Voxellst>();
        m_pVoxellstio = std::make_shared<VoxellstIO>();
    }

}

void Engine::setup(Mode mode) {

//    m_instance = appSetting.m_context.m_instance;
//    m_device = appSetting.m_context.m_device;
//    m_physicalDevice = appSetting.m_context.m_physicalDevice;
//    m_queues = appSetting.m_queues;
//
////    VkCommandPoolCreateInfo poolCreateInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
////    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
////    vkCreateCommandPool(m_device, &poolCreateInfo, nullptr, &m_cmdPool);
////
////    VkPipelineCacheCreateInfo pipelineCacheInfo{ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
////    vkCreatePipelineCache(m_device, &pipelineCacheInfo, nullptr, &m_pipelineCache);
//
//    m_queue = m_queues[eGCT].queue;
//    m_graphicsQueueIndex = m_queues[eGCT].familyIndex;
//
//    m_pAlloc = std::make_shared<Allocator>();
//    m_pAlloc->init(m_instance, m_device, m_physicalDevice);
//    m_debug.setup(m_device);
//
//    //m_pRaytracingio->setup(appSetting,m_queues[eGCT],m_graphicsQueueIndex,m_pAlloc);
//    m_pRaytracing->setup(m_pRaytracingio,appSetting,m_queues[eGCT],m_graphicsQueueIndex,m_pAlloc);

}

void Engine::input(std::string path){


 //   m_pRaytracingio->input(path);
    m_pFileio = std::make_shared<FileIO>();
    m_pFileio->readXml(path);
    m_mode = m_pFileio->m_mode;
    init(m_pFileio->m_mode);

    if(m_mode == Mode::eRaytracing) {
        m_pRaytracing->setup(appSetting, m_pRaytracingio);
        m_pRaytracing->upload(m_pFileio, m_pRaytracingio);

        m_pRaytracingio->definedDir = m_pFileio->m_pRaytracingXml->definedDir;
        m_pRaytracingio->projectDir = m_pFileio->m_pRaytracingXml->projectDir;
    }else if(m_mode == Mode::eVoxelLST){

        m_pVoxellst->setup(appSetting, m_pVoxellstio);
        m_pVoxellst->upload(m_pFileio, m_pVoxellstio);

        m_pVoxellstio->definedDir = m_pFileio->m_pVoxelLstXml->definedDir;
        m_pVoxellstio->projectDir = m_pFileio->m_pVoxelLstXml->projectDir;
    }

  //  m_pRaytracing->upload(m_fileio, m_pRaytracingio);

}

void Engine::create() {

    if(m_mode == Mode::eRaytracing) {
        m_pRaytracing->create(m_pRaytracingio);
    }else if(m_mode == Mode::eVoxelLST){
        m_pVoxellst->create(m_pVoxellstio);
    }


//    n_work = m_pRaytracingio->n_angle;
//    VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
//    allocateInfo.commandPool = m_cmdPool;
//    allocateInfo.commandBufferCount = n_work;
//    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    m_commandBuffers.resize(n_work);
//    vkAllocateCommandBuffers(m_device, &allocateInfo, m_commandBuffers.data());
//
//    // wait fences, guarantee output after calculation
//    m_waitFences.resize(n_work);
//    for (auto& fence : m_waitFences)
//    {
//        VkFenceCreateInfo fenceCreateInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
//        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//        vkCreateFence(m_device, &fenceCreateInfo, nullptr, &fence);
//    }
}

void Engine::run() {

    if(m_mode == Mode::eRaytracing)
        m_pRaytracing->run(m_pRaytracingio,m_pFileio);
    else if(m_mode == Mode::eVoxelLST)
        m_pVoxellst->run(m_pVoxellstio,m_pFileio);

}


void Engine::destroy() {

    if(m_mode == Mode::eRaytracing)
        m_pRaytracing->destroy(m_pRaytracingio);
    else if(m_mode == Mode::eVoxelLST)
        m_pVoxellst->destroy(m_pVoxellstio);
//    m_pRaytracingio->destroy();

//    for (uint32_t i = 0; i < n_work; i++)
//    {
//        vkDestroyFence(m_device, m_waitFences[i], nullptr);
//        vkFreeCommandBuffers(m_device, m_cmdPool, 1, &m_commandBuffers[i]);
//    }
 //   vkDeviceWaitIdle(m_device);
//    vkDestroyPipelineCache(m_device, m_pipelineCache, nullptr);
//    vkDestroyCommandPool(m_device, m_cmdPool, nullptr);

//    m_pAlloc->deinit();
    appSetting.destroy();


}