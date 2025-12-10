//
// Created by admin on 8/12/2025.
//

#include <iomanip>
#include "voxelization.h"

bool Voxelization::setup(AppSetting &appSetting, std::shared_ptr<VoxelizationIO> &modelio) {
    // 1. 从 AppSetting 获取上下文
    modelio->m_device         = appSetting.m_context.m_device;
    modelio->m_physicalDevice = appSetting.m_context.m_physicalDevice;
    modelio->m_instance       = appSetting.m_context.m_instance;
    modelio->m_queues         = appSetting.m_queues;
    modelio->m_queue          = appSetting.m_queues[eGCT].queue; // 假设使用图形队列
    modelio->m_queueIndex     = appSetting.m_queues[eGCT].familyIndex;

    // 2. 初始化 Allocator
    modelio->m_pAlloc = std::make_shared<Allocator>();
    modelio->m_pAlloc->init(modelio->m_instance, modelio->m_device, modelio->m_physicalDevice);

    // 3. 初始化 Command Pool
    modelio->m_genCmdBuf.init(modelio->m_device, modelio->m_queueIndex);
    modelio->m_debug.setup(modelio->m_device);

    return true;
}

bool Voxelization::upload(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelizationIO> &modelio) {
    /// 组分属性
    m_pCompo->createCompoOptical(fileio);
    modelio->m_meshio->spectrals = m_pCompo->spectrals;
    modelio->m_meshio->spectralNames = m_pCompo->spectralNames;
    modelio->m_meshio->thermals = m_pCompo->thermals;
    modelio->m_meshio->thermalNames = m_pCompo->thermalNames;

    /// 场景
    m_pScene->createObjScene(fileio, m_pCompo);
    modelio->m_meshio->objMeshes = m_pScene->objMeshes;
    modelio->m_meshio->meshLinks = m_pScene->meshLinks;
    modelio->m_instanceio->instances = m_pScene->instances;
    modelio->m_instanceio->instanceLinks = m_pScene->instanceLinks;

    /// 观测几何
    m_pGeometry->createGeometry(fileio);
    modelio->angles = m_pGeometry->angles;
    modelio->waves = m_pGeometry->waves;
    modelio->sensor = m_pGeometry->sensor;
    modelio->light = m_pGeometry->light;

    /// RT setting
    modelio->rtsetting.imageSize = m_pGeometry->imageSize;
    modelio->rtsetting.n_wave = fileio->m_sensorxml.waves.size();
    modelio->rtsetting.isTemperature =  fileio->m_sensorxml.isTemperature;
    modelio->rtsetting.isDisplay = fileio->m_sensorxml.isDisplay;
    modelio->rtsetting.maxDepth = fileio->m_settingxml.maxDepth;
    modelio->rtsetting.n_sample = fileio->m_settingxml.n_sample;

    /// output 输出setting
    modelio->ouputsetting.isAlbedo = fileio->m_sensorxml.isAlbedo;
    modelio->ouputsetting.isImage = fileio->m_sensorxml.isImage;
    modelio->ouputsetting.isOrth = fileio->m_sensorxml.isOrth;


    float x = fileio->m_scenexml.background.sceneSize.x; // lenght
    float y = fileio->m_scenexml.background.sceneSize.y; // width
    float z = fileio->m_scenexml.background.sceneSize.z; // height
    modelio->setting.voxelSize = 1.0;
    modelio->setting.volumeSize = glm::vec3(x,z,y);


    /// 设置路径
    modelio->definedDir = fileio->definedDir;
    modelio->projectDir = fileio->projectDir;

    return true;
}

bool Voxelization::create(std::shared_ptr<VoxelizationIO> &modelio) {
    m_pBuffer->create(modelio);
    m_pDescriptor->create(modelio);
    m_pPipeline->create(modelio);
    m_pCommand->create(modelio);
    return true;
}

bool Voxelization::run(std::shared_ptr<VoxelizationIO> &modelio) {
    return m_pCommand->run(modelio);
}

void Voxelization::destroy(std::shared_ptr<VoxelizationIO> &modelio) {
    vkDeviceWaitIdle(modelio->m_device);

    m_pCommand->destroy(modelio);
    m_pPipeline->destroy(modelio);
    m_pDescriptor->destroy(modelio);
    m_pBuffer->destroy(modelio);

    modelio->m_genCmdBuf.deinit();
    modelio->m_pAlloc->deinit(); // 销毁分配器
}

void Voxelization::debugOutput(std::shared_ptr<VoxelizationIO>& modelio) {
    auto& device = modelio->m_device;
    auto& alloc = modelio->m_pAlloc;
    auto& cmdPool = modelio->m_genCmdBuf; // 使用通用的命令池
    auto& setting = modelio->setting;

    // 1. 计算总大小
    uint32_t width = setting.gridSize.x;
    uint32_t height = setting.gridSize.y;
    uint32_t depth = setting.gridSize.z;
    VkDeviceSize size = width * height * depth * sizeof(uint32_t); // R32_UINT = 4 bytes

    // 2. 创建一个 CPU 可见的临时 Buffer 用于接收数据
    // 使用 NVVK 或原生 Vulkan 创建
    VkBufferCreateInfo bufInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufInfo.size = size;
    bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    // 使用 HOST_VISIBLE | HOST_COHERENT 确保 CPU 能直接读
    nvvk::Buffer stageBuf = alloc->createBuffer(bufInfo,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // 3. 执行 Copy 命令 (Image -> Buffer)
    {
        VkCommandBuffer cmd = cmdPool.createCommandBuffer();

        // 3.1 确保 Image 处于 TRANSFER_SRC 状态 (如果是 GENERAL 也可以直接用)
        // 这里假设它已经是 GENERAL

        VkBufferImageCopy region = {};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageExtent = {width, height, depth};

        vkCmdCopyImageToBuffer(cmd, modelio->m_voxelTexture.image, VK_IMAGE_LAYOUT_GENERAL, stageBuf.buffer, 1, &region);

        // 3.2 插入内存屏障，确保 CPU 读取前 GPU 已经写完 Buffer
        VkBufferMemoryBarrier barrier = {VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
        barrier.buffer = stageBuf.buffer;
        barrier.size = VK_WHOLE_SIZE;

        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT,
                             0, 0, nullptr, 1, &barrier, 0, nullptr);

        cmdPool.submitAndWait(cmd);
    }

    // 4. 映射内存并读取
    void* data = alloc->map(stageBuf);
    uint32_t* voxels = static_cast<uint32_t*>(data);

    std::cout << "--- Voxelization Debug Check ---" << std::endl;
    size_t count = 0;

    // 遍历所有体素
    for (uint32_t z = 0; z < depth; z++) {
        for (uint32_t y = 0; y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                // 计算一维索引
                size_t index = z * (width * height) + y * width + x;
                uint32_t val = voxels[index];

                if (val > 0) {
                    count++;
                    // 只打印前 20 个找到的体素，防止刷屏
                    if (count < 20) {
                        std::cout << "Voxel at [" << x << ", " << y << ", " << z << "] = ID: " << val << std::endl;
                    }
                }
            }
        }
    }

    std::cout << "Total occupied voxels: " << count << std::endl;
    std::cout << "Occupancy rate: " << (float)count / (width * height * depth) * 100.0f << "%" << std::endl;

    alloc->unmap(stageBuf);
    alloc->destroy(stageBuf);
}