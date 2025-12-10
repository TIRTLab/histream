//
// Created by admin on 8/12/2025.
//

#ifndef STREAM_VK_VOXELIZATIONIO_H
#define STREAM_VK_VOXELIZATIONIO_H

#include <vulkan/vulkan.hpp>
#include <nvvk/resourceallocator_vk.hpp>
#include <nvvk/context_vk.hpp>
#include <nvvk/commands_vk.hpp>
#include <nvvk/debug_util_vk.hpp>
#include <nvvk/descriptorsets_vk.hpp>
#include <nvvk/sbtwrapper_vk.hpp>
//#include <nvvk/raytraceKHR_vk.hpp>

#include "src/base/accelstruct.h"
#include "src/base/meshio.h"
#include "src/base/instanceio.h"
#include "src/base/virtualio.h"
#include "src/base/fileio.h"
#include "src/base/structs.h"
#include "src/base/structs_cg.h"
#include "src/base/queue.h"
#include "src/base/appsetting.h"



// #define ALLOC_DMA  <--- This is in the CMakeLists.txt
#include "nvvk/resourceallocator_vk.hpp"
#if defined(ALLOC_DMA)
#include <nvvk/memallocator_dma_vk.hpp>
typedef nvvk::ResourceAllocatorDma Allocator;
#elif defined(ALLOC_VMA)
#include <nvvk/memallocator_vma_vk.hpp>
typedef nvvk::ResourceAllocatorVma Allocator;
#else
typedef nvvk::ResourceAllocatorDedicated Allocator;
#endif



class VoxelizationIO {

public:
    VoxelizationIO(){
        // [新增] 构造函数中初始化数据持有者
        m_meshio = std::make_shared<MeshIO>();
        m_instanceio = std::make_shared<InstanceIO>();
        m_virtualio = std::make_shared<VirtualIO>();
    };

    // ----------------------------------------------------------------
    // 基础 Context (必须自己持有)
    // ----------------------------------------------------------------
    VkDevice                       m_device;
    VkPhysicalDevice               m_physicalDevice;
    VkInstance                     m_instance;
    std::shared_ptr<Allocator>     m_pAlloc;       // 资源分配器
    nvvk::DebugUtil                m_debug;
    std::vector<nvvk::Queue>       m_queues;
    VkQueue                        m_queue;
    uint32_t                       m_queueIndex{ 0 };

    // ----------------------------------------------------------------
    // 场景数据 (必须自己持有，不能依赖 RaytracingIO)
    // ----------------------------------------------------------------
    std::shared_ptr<MeshIO>        m_meshio;
    std::shared_ptr<InstanceIO>    m_instanceio;
    std::shared_ptr<VirtualIO>     m_virtualio;

    // 场景包围盒信息 (从 upload 阶段获取)
    glm::vec3 sceneSize;
    glm::vec3 sceneOrigin;

    // 传感器信息 (从 upload 阶段获取)
    SensorMatrix sensor;
    LightSet     light;
    std::vector<Angle> angles;
    std::vector<float> waves;

    // setting 光线追踪设置信息 (从 upload 阶段获取)
    RayRTSetting rtsetting;

    // 输出设置
    OutputSeting ouputsetting;


    // 路径信息
    std::string projectDir;
    std::string definedDir;

    std::shared_ptr<nvvk::Buffer> m_pBufferWave;
    std::shared_ptr<nvvk::Buffer> m_pBufferSensor;
    std::shared_ptr<nvvk::Buffer> m_pBufferLight;
    std::shared_ptr<AccelStruct>  m_pAccelStruct;

    // ----------------------------------------------------------------
    // 体素化独有资源
    // ----------------------------------------------------------------
    nvvk::Texture m_voxelTexture;

    // 管线资源
    VkRenderPass     m_renderPass{ VK_NULL_HANDLE };
    VkFramebuffer    m_framebuffer{ VK_NULL_HANDLE };
    VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };
    VkPipeline       m_pipeline{ VK_NULL_HANDLE };

    // 描述符
    nvvk::DescriptorSetBindings m_bindings;
    VkDescriptorPool            m_descPool{ VK_NULL_HANDLE };
    VkDescriptorSet             m_descSet{ VK_NULL_HANDLE };
    VkDescriptorSetLayout       m_descSetLayout{ VK_NULL_HANDLE };

    // 命令与同步
    nvvk::CommandPool m_genCmdBuf;
    VkFence           m_fence;

    // 设置
    VoxelSetting setting;
};
#endif //STREAM_VK_VOXELIZATIONIO_H
