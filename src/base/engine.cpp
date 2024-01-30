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
        m_pVoxellist = std::make_shared<Voxellst>();
        m_pVoxellistio = std::make_shared<VoxellstIO>();
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

        m_pRaytracingio->shaderDir = m_pFileio->m_pRaytracingXml->shaderDir;
        m_pRaytracingio->projectDir = m_pFileio->m_pRaytracingXml->projectDir;
    }else if(m_mode == Mode::eVoxelLST){

        m_pRaytracing->setup(appSetting, m_pRaytracingio);
        m_pRaytracing->upload(m_pFileio, m_pRaytracingio);

        m_pRaytracingio->shaderDir = m_pFileio->m_pRaytracingXml->shaderDir;
        m_pRaytracingio->projectDir = m_pFileio->m_pRaytracingXml->projectDir;
    }

  //  m_pRaytracing->upload(m_fileio, m_pRaytracingio);

}

void Engine::create() {

    m_pRaytracing->create(m_pRaytracingio);

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

    m_pRaytracing->run(m_pRaytracingio,m_pFileio);
//    for (int kangle = 0; kangle < n_work; kangle++)
//    {
//        VkResult result = vkWaitForFences(m_device, 1, &m_waitFences[kangle], VK_TRUE, 10000000000);
//
//        const VkCommandBuffer &cmdBuf = m_commandBuffers[kangle];
//        VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
//        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//        vkBeginCommandBuffer(cmdBuf, &beginInfo);
//
//        glm::vec4 angles = m_pRaytracingio->m_sceneio->angles[kangle];
//        std::cout << "Angle Info:"
//                  << "    vza_" << std::to_string(angles.x) << "    vaa_" << std::to_string(angles.y)
//                  << "    sza_" << std::to_string(angles.z) << "    saa_" << std::to_string(angles.w) << std::endl;
//
//        float ratio = 1.0;
//        //ratio = 0.707;
//        SensorMatrix sensorMatrix =  m_pRaytracing->m_pGeometry->createSensor(m_pRaytracingio, angles.x, angles.y, ratio);
//        m_pRaytracingio->updateSensor(sensorMatrix);
//
//        LightSet lightSet = m_pRaytracing->m_pGeometry->createLight(angles.z, angles.w,
//                                                                  m_pRayTracerIO->m_pRtInput->waveSets[0].direct,
//                                                                  m_pRayTracerIO->m_pRtInput->waveSets[0].diffuse);
//        m_pRayTracerIO->updateLight(lightSet);
//
//        m_pRayTracerIO->updateSetting(m_pRayTracerIO->m_pSetting);
//
//        m_pRayTracer->run(cmdBuf, m_size);
//        vkEndCommandBuffer(cmdBuf);
//
//        // pipeline stage at which the queue submission will wait
//        vkResetFences(m_device, 1, &m_waitFences[kangle]);
//        const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
//        VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
//        submitInfo.pWaitDstStageMask = &waitStageMask;
//        submitInfo.pCommandBuffers = &m_commandBuffers[kangle];
//        submitInfo.commandBufferCount = 1;
//        vkQueueSubmit(m_queue, 1, &submitInfo, m_waitFences[kangle]);
//
//        // output
//        output(angles);
//
//        std::cout << "Success: " << kangle << std::endl;
//    }
//
//    m_pRaytracing->run()

}


void Engine::destroy() {

    m_pRaytracing->destroy(m_pRaytracingio);
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