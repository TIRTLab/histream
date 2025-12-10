//
// Created by Fan Tengyuan on 9/12/2025.
//
#include "buffer_voxelization.h"

bool Buffer_voxelization::create(std::shared_ptr<VoxelizationIO> &modelio) {
    auto &m_device = modelio->m_device;
    auto &m_pAlloc = modelio->m_pAlloc;
    auto &meshio = modelio->m_meshio;
    auto &instanceio = modelio->m_instanceio;
    auto &setting = modelio->setting;

    nvvk::CommandPool cmdGen(m_device, modelio->m_queueIndex);

    // ----------------------------------------------------------------
    // 1. 上传几何数据 (ObjMesh -> Vertex/Index Buffer)
    // ----------------------------------------------------------------
    if (meshio->m_bufferMeshes.empty()) {
        VkCommandBuffer cmd = cmdGen.createCommandBuffer();

        for (size_t i = 0; i < meshio->objMeshes.size(); i++) {
            MeshBuffer meshBuffer;
            meshBuffer.nbVertices = static_cast<uint32_t>(meshio->objMeshes[i].nVertices);
            meshBuffer.nbIndices  = static_cast<uint32_t>(meshio->objMeshes[i].nIndices);

            // 创建 Vertex Buffer
            meshBuffer.vertexBuffer = m_pAlloc->createBuffer(cmd, meshio->objMeshes[i].vertices,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

            // 创建 Index Buffer
            meshBuffer.indexBuffer = m_pAlloc->createBuffer(cmd, meshio->objMeshes[i].indices,
                VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

            meshio->m_bufferMeshes.push_back(meshBuffer);
        }
        cmdGen.submitAndWait(cmd);
    }

    // ----------------------------------------------------------------
    // 2. 创建 3D 体素纹理 (Storage Image)
    // ----------------------------------------------------------------

    // 2.1 计算网格分辨率
    setting.gridSize = glm::uvec3(
        static_cast<uint32_t>(std::ceil(setting.volumeSize.x / setting.voxelSize)),
        static_cast<uint32_t>(std::ceil(setting.volumeSize.y / setting.voxelSize)),
        static_cast<uint32_t>(std::ceil(setting.volumeSize.z / setting.voxelSize))
    );
    setting.gridSize = glm::max(setting.gridSize, glm::uvec3(1)); // 保证最小 1x1x1

    // 2.2 定义 VkImageCreateInfo
    VkImageCreateInfo imageInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageInfo.imageType = VK_IMAGE_TYPE_3D;
    imageInfo.extent.width  = setting.gridSize.x;
    imageInfo.extent.height = setting.gridSize.y;
    imageInfo.extent.depth  = setting.gridSize.z;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R32_UINT; // R32UI 用于 Atomic 操作
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // 必须包含 STORAGE (写入) 和 TRANSFER (清零/拷贝)
    imageInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    // 2.3 第一步：创建 Image (分配显存)
    // nvvk::ResourceAllocator 应该有 createImage 方法
    nvvk::Image voxelImage = m_pAlloc->createImage(imageInfo);

    // 2.4 定义 VkImageViewCreateInfo
    // 必须显式指定为 3D View，否则 Vulkan 可能无法正确识别
    VkImageViewCreateInfo viewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    viewInfo.pNext = nullptr;
    viewInfo.image = voxelImage.image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D; // <--- 关键：指定 3D 类型
    viewInfo.format = imageInfo.format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    // 2.5 第二步：创建 Texture (封装 Image + 创建 View)
    // 调用你查到的: createTexture(const Image& image, const VkImageViewCreateInfo& imageViewCreateInfo)
    modelio->m_voxelTexture = m_pAlloc->createTexture(voxelImage, viewInfo);

    // 2.6 手动修正 Descriptor Layout
    // createTexture 默认会将 descriptor.imageLayout 设置为 SHADER_READ_ONLY_OPTIMAL
    // 但我们需要 STORAGE_IMAGE，所以必须改为 GENERAL
    modelio->m_voxelTexture.descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    // ----------------------------------------------------------------
    // 3. 图像布局转换 (Undefined -> General)
    // ----------------------------------------------------------------
    {
        VkCommandBuffer cmd = cmdGen.createCommandBuffer();
        nvvk::cmdBarrierImageLayout(cmd, modelio->m_voxelTexture.image,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT);
        cmdGen.submitAndWait(cmd);
    }

    m_pAlloc->finalizeAndReleaseStaging();

    // ///--------------------------------------------------------------------
    // ///  component properties
    // ///--------------------------------------------------------------------
    //
    // VkDevice & m_device = modelio->m_device;
    // nvvk::Queue &m_queue  = modelio->m_queues[eGCT];
    // auto & meshio = modelio->m_meshio;
    // auto & instanceio = modelio->m_instanceio;
    // auto & virtualio = modelio->m_virtualio;
    // auto & m_pAlloc = modelio->m_pAlloc;
    // auto & m_pAccelStruct = modelio->m_pAccelStruct;
    //
    // nvvk::CommandPool cmdGen(m_device, m_queue.familyIndex);
    //
    // if (!meshio->spectrals.empty())
    // {
    //     VkCommandBuffer cmdBufSpectral = cmdGen.createCommandBuffer();
    //     VkBufferUsageFlags     usage_ = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    //     meshio->m_pBufferSpectral = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufSpectral, meshio->spectrals, usage_));
    //     cmdGen.submitAndWait(cmdBufSpectral);
    // }
    //
    // if (!meshio->thermals.empty())// no utilise the isTemperature
    // {
    //     VkCommandBuffer cmdBufThermal = cmdGen.createCommandBuffer();
    //     meshio->m_pBufferThermal = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufThermal, meshio->thermals, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
    //     cmdGen.submitAndWait(cmdBufThermal);
    // }
    //
    // if (!modelio->waves.empty())
    // {
    //     VkCommandBuffer cmdBufWave = cmdGen.createCommandBuffer();
    //     modelio->m_pBufferWave = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufWave, modelio->waves, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
    //     cmdGen.submitAndWait(cmdBufWave);
    // }
    //
    // ///--------------------------------------------------------------------
    // ///  Scene properties
    // ///--------------------------------------------------------------------
    // // models buffer
    // nvvk::CommandPool cmdBufGet(m_device, m_queue.familyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_queue.queue);
    // for (int kmodel = 0; kmodel < meshio->objMeshes.size(); kmodel++)
    // {
    //
    //     VkCommandBuffer cmdBufModel = cmdBufGet.createCommandBuffer();
    //     MeshBuffer meshbuffer;
    //     meshbuffer.nbVertices = static_cast<uint32_t>(meshio->objMeshes[kmodel].nVertices);
    //     meshbuffer.nbIndices = static_cast<uint32_t>(meshio->objMeshes[kmodel].nIndices);
    //
    //     meshbuffer.vertexBuffer =
    //             m_pAlloc->createBuffer(cmdBufModel, meshio->objMeshes[kmodel].vertices,
    //                                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
    //                                    | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
    //     meshbuffer.indexBuffer =
    //             m_pAlloc->createBuffer(cmdBufModel, meshio->objMeshes[kmodel].indices,
    //                                    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
    //                                    | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
    //     cmdBufGet.submitAndWait(cmdBufModel);
    //     //m_pAlloc->finalizeAndReleaseStaging();
    //     meshio->m_bufferMeshes.emplace_back(meshbuffer);
    // }
    //
    // // modelLink
    // VkCommandBuffer cmdBufModelLink = cmdGen.createCommandBuffer();
    // for (int kmodel = 0; kmodel < meshio->objMeshes.size(); kmodel++)
    // {
    //
    //     meshio->meshLinks[kmodel].vertexAddress = nvvk::getBufferDeviceAddress(m_device, meshio->m_bufferMeshes[kmodel].vertexBuffer.buffer);
    //     meshio->meshLinks[kmodel].indexAddress = nvvk::getBufferDeviceAddress(m_device, meshio->m_bufferMeshes[kmodel].indexBuffer.buffer);
    // }
    // meshio->m_pBufferMeshLink = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufModelLink, meshio->meshLinks, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
    // cmdGen.submitAndWait(cmdBufModelLink);
    //
    // // instanceLink : add address first and then creat
    // VkCommandBuffer cmdBufLink = cmdGen.createCommandBuffer();
    // if (!instanceio->instanceLinks.empty())
    //     instanceio->m_pBufferInstanceLink = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufLink, instanceio->instanceLinks, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
    // cmdGen.submitAndWait(cmdBufLink);
    //
    // ///--------------------------------------------------------------------
    // ///  Accelerate properties
    // ///--------------------------------------------------------------------
    // m_pAccelStruct->createAccelStruct(m_device,meshio,instanceio);
    //
    //
    // ///--------------------------------------------------------------------
    // ///  geometry properties
    // ///--------------------------------------------------------------------
    // VkCommandBuffer cmdBufSensor = cmdGen.createCommandBuffer();
    // modelio->m_pBufferSensor = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufSensor,
    //                                                                                  sizeof(SensorMatrix),
    //                                                                                  &modelio->sensor,
    //                                                                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    // cmdGen.submitAndWait(cmdBufSensor);
    //
    // VkCommandBuffer cmdBufLight = cmdGen.createCommandBuffer();
    // modelio->m_pBufferLight = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufLight,
    //                                                                                 sizeof(LightSet),
    //                                                                                 &modelio->light,
    //                                                                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
    // cmdGen.submitAndWait(cmdBufLight);
    //
    // ///--------------------------------------------------------------------
    // ///  Virtual Sceen properties
    // ///--------------------------------------------------------------------
    // VkCommandBuffer cmdBufStorage = cmdGen.createCommandBuffer();
    // int outputSize = modelio->rtsetting.imageSize.x * modelio->rtsetting.imageSize.y * modelio->rtsetting.n_wave;
    // std::vector<float> outputImage(outputSize, 0.0);
    // virtualio->m_pBufferStorage = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufStorage, outputImage, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
    // cmdGen.submitAndWait(cmdBufStorage);
    //
    //
    //
    //
    // m_pAlloc->finalizeAndReleaseStaging();
    return false;
}

void Buffer_voxelization::destroy(std::shared_ptr<VoxelizationIO>& modelio)
{
    auto &m_pAlloc = modelio->m_pAlloc;
    auto &meshio   = modelio->m_meshio;

    // ----------------------------------------------------------------
    // 1. 销毁 3D 体素纹理
    // ----------------------------------------------------------------
    // nvvk::Allocator::destroy(Texture) 会自动做两件事：
    // 1. 调用 vkDestroyImageView 销毁 texture.descriptor.imageView
    // 2. 调用 vkFreeMemory/vkDestroyImage 销毁 texture.image
    // 因为我们在 create 中将手动创建的 View 句柄赋给了 descriptor.imageView，
    // 所以这里直接调用 destroy 是安全的。
    if (modelio->m_voxelTexture.image != VK_NULL_HANDLE) {
        m_pAlloc->destroy(modelio->m_voxelTexture);

        // 置空防止悬空指针
        modelio->m_voxelTexture = {};
    }

    // ----------------------------------------------------------------
    // 2. 销毁几何数据 Buffer
    // ----------------------------------------------------------------
    // 遍历所有已上传的 MeshBuffer并释放
    for (auto& meshBuffer : meshio->m_bufferMeshes) {
        // destroy 内部会检查是否为 VK_NULL_HANDLE，所以直接调用是安全的
        m_pAlloc->destroy(meshBuffer.vertexBuffer);
        m_pAlloc->destroy(meshBuffer.indexBuffer);
    }

    // 清空列表，避免析构后再次访问
    meshio->m_bufferMeshes.clear();

    // ----------------------------------------------------------------
    // 3. 销毁其他潜在 Buffer (如果在 create 中添加了)
    // ----------------------------------------------------------------
    // 如果你在 create 中恢复了 spectral/thermal 等 buffer 的创建，
    // 需要在这里解开注释进行销毁：
    /*
    if (meshio->m_pBufferSpectral) {
        m_pAlloc->destroy(*meshio->m_pBufferSpectral);
        meshio->m_pBufferSpectral = nullptr;
    }
    if (modelio->m_pBufferWave) {
        m_pAlloc->destroy(*modelio->m_pBufferWave);
        modelio->m_pBufferWave = nullptr;
    }
    // ... 其他 buffer
    */
}

