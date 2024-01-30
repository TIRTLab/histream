//
// Created by admin on 2024/1/25.
//

#ifndef FIELD_ENGINE_H
#define FIELD_ENGINE_H

#include "src/raytracing/raytracingio.h"
#include "src/raytracing/raytracing.h"
#include "src/voxelist/voxellst.h"
#include "src/voxelist/voxellstio.h"
#include "src/base/appsetting.h"
#include "src/base/fileio.h"




class Engine {
public:
    Engine(){};

    void input(std::string path);
    void init(Mode mode);
    void setup(Mode mode);

    void create();
    void run();
    void destroy();

    Mode m_mode;
    AppSetting appSetting;
    std::shared_ptr<FileIO>       m_pFileio;
    std::shared_ptr<Raytracing>   m_pRaytracing;
    std::shared_ptr<RaytracingIO> m_pRaytracingio;
    std::shared_ptr<Voxellst>    m_pVoxellist;
    std::shared_ptr<VoxellstIO>  m_pVoxellistio;

//    int n_work;
//    VkInstance m_instance;
//    VkDevice m_device;
//    VkPhysicalDevice m_physicalDevice;
//    VkQueue m_queue;
//    std::vector<nvvk::Queue> m_queues;
//    std::shared_ptr<Allocator> m_pAlloc;
//
//    uint32_t m_graphicsQueueIndex{ VK_QUEUE_FAMILY_IGNORED };
//    nvvk::DebugUtil m_debug;

//    VkCommandPool m_cmdPool{ VK_NULL_HANDLE };
//    VkPipelineCache m_pipelineCache{ VK_NULL_HANDLE };
//    VkExtent2D m_size{ 0, 0 };
//    std::vector<VkCommandBuffer> m_commandBuffers;
//    std::vector<VkFence> m_waitFences;


};


#endif //FIELD_ENGINE_H
