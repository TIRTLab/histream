//
// Created by ShIP on 2024/12/2.
//
#include <iomanip>
#include "buildingshadow.h"

bool BuildingShadow::setup( AppSetting &appsetting, std::shared_ptr<BuildingShadowIO> &modelio){


    modelio->m_device = appsetting.m_context.m_device;
    modelio->m_physicalDevice = appsetting.m_context.m_physicalDevice;
    modelio->m_instance = appsetting.m_context.m_instance;
    modelio->m_queues = appsetting.m_queues;
    modelio->m_queue =  modelio->m_queues[eGCT].queue;
    modelio->m_queueIndex = modelio->m_queues[eGCT].familyIndex;
    //    m_instance = appSetting.m_context.m_instance;
//    m_device = appSetting.m_context.m_device;
//    m_physicalDevice = appSetting.m_context.m_physicalDevice;
//    m_queues = appSetting.m_queues;


//    VkCommandPoolCreateInfo poolCreateInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
//    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//    vkCreateCommandPool(raytracingio->m_device, &poolCreateInfo, nullptr, &raytracingio->m_cmdPool);

    modelio->m_genCmdBuf.init(modelio->m_device,modelio->m_queueIndex);


//    VkPipelineCacheCreateInfo pipelineCacheInfo{ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
//    vkCreatePipelineCache(raytracingio->m_device, &pipelineCacheInfo, nullptr, &raytracingio->m_pipelineCache);

    modelio->m_pAlloc  = std::make_shared<Allocator>();
    modelio->m_pAlloc->init(modelio->m_instance, modelio->m_device, modelio->m_physicalDevice);
    modelio->m_debug.setup(modelio->m_device);

    /// requesting ray tracing properties. from physical device.
    VkPhysicalDeviceProperties2 rayTracingProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
    rayTracingProperties.pNext = &(modelio->m_rtProperties);
    vkGetPhysicalDeviceProperties2(modelio->m_physicalDevice, &rayTracingProperties);

    // if (modelio->useSBTWrapper)
    // {
    //     modelio->m_sbtWrapper.setup(modelio->m_device, modelio->m_queueIndex, modelio->m_pAlloc.get(), modelio->m_rtProperties);
    // }

    modelio->m_pAccelStruct->m_rtBuilder.setup(modelio->m_device, modelio->m_pAlloc.get(),modelio->m_queueIndex);

    return true;

}

bool BuildingShadow::upload(std::shared_ptr<FileIO>& fileio, std::shared_ptr<BuildingShadowIO>& modelio)
{

    modelio->projectDir = fileio->m_pBuildingShadowXml->projectDir;
    modelio->defineDir = fileio->m_pBuildingShadowXml->defineDir;
    m_pGeometry->createGeometry(fileio,modelio);
    m_pScene->createObjScene(fileio, modelio);
    // uploadSetting(fileio, modelio);

    return true;
}

bool BuildingShadow::create(std::shared_ptr<BuildingShadowIO>& modelio)
{
    m_pBuffer->createBuffer(modelio);
    m_pDescriptor->createDescriptor(modelio);
    m_pPipeline->createPipeline(modelio);
    m_pCommand->create(modelio);
    return true;
}

bool BuildingShadow::run(std::shared_ptr<BuildingShadowIO>& modelio, std::shared_ptr<FileIO>& fileio)
{
    m_pCommand->run(modelio);

    outputFacetInfo(modelio,fileio);
    return true;
}

bool BuildingShadow::destroy(std::shared_ptr<BuildingShadowIO>& modelio)
{
    m_pBuffer->destroy(modelio);
    modelio->m_sbtWrapper.destroy();
    modelio->m_pAccelStruct->m_rtBuilder.destroy();

    m_pDescriptor->destroy(modelio);
    m_pPipeline->destroy(modelio);
    m_pCommand->destroy(modelio);



    vkDeviceWaitIdle(modelio->m_device);
    modelio->m_genCmdBuf.deinit();
    modelio->m_pAlloc->deinit();

    return false;
}

void BuildingShadow::outputFacetInfo(std::shared_ptr<BuildingShadowIO>& modelio, std::shared_ptr<FileIO>& fileio)
{
    VkBufferUsageFlags usage{VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT};
    int n_facet = modelio->n_facet;
    VkDeviceSize bufferSize = n_facet * sizeof(FacetInfo);
    nvvk::Buffer pixelBuffer = modelio->m_pAlloc->createBuffer(bufferSize, usage,
                                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    m_pVirtual->bufferToBuffer(modelio, *(modelio->m_pFacetInfoBuffer), bufferSize, pixelBuffer);

    // write the buffer to disk
    void *data = modelio->m_pAlloc->map(pixelBuffer);
    float *pData = reinterpret_cast<float *>(data);

    float test0 = pData[1];
    float test3 = pData[5];
    float test5 = pData[9];
    float test7 = pData[13];
    float test9 = pData[17];
    //     float test6 = pData[10000];
    //     float test10 = pData[250000];
    //std::cout << "value: " << test0 << std::endl;

    // 打开文件
    std::ofstream outputFile(fileio->m_pBuildingShadowXml->projectDir + "/output_sunlitArea.txt");
    if (!outputFile.is_open()) {
        std::cerr << "无法打开文件: " << fileio->m_pBuildingShadowXml->projectDir + "/output_sunlitArea.txt" << std::endl;
        return;
    }
    // 写入数据
    for (size_t i = 0; i < modelio->n_facet; ++i) {
        // outputFile << pData[2*i] << " ";  // 写入整数
        outputFile << std::fixed << std::setprecision(2) << pData[2*i + 1] << "\n";  // 写入保留两位小数的浮动数值
    }
    // 关闭文件
    outputFile.close();

    modelio->m_pAlloc->unmap(pixelBuffer);
    modelio->m_pAlloc->destroy(pixelBuffer);
}
