//
// Created by ShIP on 2024/12/2.
//

#ifndef FIELD_BUILDINGSHADOWIO_H
#define FIELD_BUILDINGSHADOWIO_H



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

class BuildingShadowIO{
public:
    BuildingShadowIO(){
        m_meshio = std::make_shared<MeshIO>();
        m_instanceio = std::make_shared<InstanceIO>();
        m_pAccelStruct = std::make_shared<AccelStruct>();  // method and resource, have to make
    }
    //        m_virtualio = std::make_shared<VirtualIO>();
    //        m_surfio = std::make_shared<SurfIO>();


    std::string projectDir;
    std::string defineDir;

    BuildingShadowSetting setting;

    int n_facet;

    Angle angle;
    std::shared_ptr<MeshIO> m_meshio;
    std::shared_ptr<InstanceIO> m_instanceio;

    // initialization
    VkDevice                       m_device{};
    VkPhysicalDevice               m_physicalDevice{};
    VkInstance                     m_instance{};
    std::shared_ptr<Allocator>     m_pAlloc;
    nvvk::DebugUtil                m_debug;
    std::vector<nvvk::Queue>       m_queues;
    VkQueue                        m_queue{};
    uint32_t                       m_queueIndex{ 0 };
    VkFence m_fence{};

    int m_currentSemaphore;
    std::vector<VkSemaphore> m_semaphores;
    int semaphoresNum{ 0 };
    // create
    nvvk::CommandPool   m_genCmdBuf;

    std::shared_ptr<AccelStruct>  m_pAccelStruct;
    std::shared_ptr<nvvk::Buffer> m_pFacetInfoBuffer;
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_rtProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR };
    //        std::vector<VkPipeline> m_pipelines_shadow;

    nvvk::DescriptorSetBindings m_bindings;
    VkDescriptorPool m_descPool{ VK_NULL_HANDLE };
    VkDescriptorSet m_descSet{ VK_NULL_HANDLE };
    VkDescriptorSetLayout m_descSetLayout{ VK_NULL_HANDLE };

    std::map<BuildingShadowStage, VkPipeline> m_pipelines;
    VkPipelineLayout m_pipelineLayout { VK_NULL_HANDLE };
    bool useSBTWrapper{ true };
    bool useDeferred{ false };
    nvvk::SBTWrapper m_sbtWrapper;


    int n_pipeline = 1;

};
#endif //BUILDINGSHADOWIO_H
