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
    modelio->rtsetting.n_wave = fileio->sensorxml.waves.size();
    modelio->rtsetting.isTemperature =  fileio->sensorxml.isTemperature;
    modelio->rtsetting.isDisplay = fileio->sensorxml.isDisplay;
    modelio->rtsetting.maxDepth = fileio->settingxml.maxDepth;
    modelio->rtsetting.n_sample = fileio->settingxml.n_sample;

    /// output 输出setting
    modelio->ouputsetting.isAlbedo = fileio->sensorxml.isAlbedo;
    modelio->ouputsetting.isImage = fileio->sensorxml.isImage;
    modelio->ouputsetting.isOrth = fileio->sensorxml.isOrth;

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

bool Voxelization::destroy(std::shared_ptr<VoxelizationIO> &modelio) {
    vkDeviceWaitIdle(modelio->m_device);

    m_pCommand->destroy(modelio);
    m_pPipeline->destroy(modelio);
    m_pDescriptor->destroy(modelio);
    m_pBuffer->destroy(modelio);

    // 销毁 Mesh Buffer
    for(auto& mesh : voxelIO->m_meshio->m_bufferMeshes) {
        voxelIO->m_pAlloc->destroy(mesh.vertexBuffer);
        voxelIO->m_pAlloc->destroy(mesh.indexBuffer);
    }

    voxelIO->m_genCmdBuf.deinit();
    voxelIO->m_pAlloc->deinit(); // 销毁分配器

    return true;
}