//
// Created by admin on 2024/1/26.
//

#include "voxeleb.h"



bool Voxeleb::setup(AppSetting &appsetting, std::shared_ptr<VoxelebIO> &modelio){


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
//    vkCreateCommandPool(modelio->m_device, &poolCreateInfo, nullptr, &modelio->m_cmdPool);

    modelio->m_genCmdBuf.init(modelio->m_device,modelio->m_queueIndex);


//    VkPipelineCacheCreateInfo pipelineCacheInfo{ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
//    vkCreatePipelineCache(modelio->m_device, &pipelineCacheInfo, nullptr, &modelio->m_pipelineCache);

    modelio->m_pAlloc  = std::make_shared<Allocator>();
    modelio->m_pAlloc->init(modelio->m_instance, modelio->m_device, modelio->m_physicalDevice);
    modelio->m_debug.setup(modelio->m_device);

    VkPhysicalDeviceProperties2 rayTracingProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
    rayTracingProperties.pNext = &(modelio->m_rtProperties);
    vkGetPhysicalDeviceProperties2(modelio->m_physicalDevice, &rayTracingProperties);

    if (modelio->useSBTWrapper)
    {
        modelio->m_sbtWrapper.setup(modelio->m_device, modelio->m_queueIndex, modelio->m_pAlloc.get(), modelio->m_rtProperties);
    }

    modelio->m_pAccelStruct->m_rtBuilder.setup(modelio->m_device, modelio->m_pAlloc.get(),modelio->m_queueIndex);



    return true;

}




bool Voxeleb::upload(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelebIO> &modelio){

    // auto & fileio = modelio->m_fileio;
    // auto & meshio = modelio->m_meshio;

    uploadDefined(fileio,modelio);
    m_pCompo->createCompProperty(fileio, modelio);
    m_pScene->createPrimObjScene(fileio, modelio);
    m_pGeometry->createGeometry(fileio,modelio);
//    defineOPO(modelio);
    uploadMeteo(fileio,modelio);
    uploadSetting(fileio, modelio);
    uploadAero(fileio,modelio);

    return true;
}

bool Voxeleb::uploadSetting(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelebIO> &modelio) {

    modelio->n_wave = fileio->m_pVoxelebXml->sensorxml.waves.size();
    modelio->n_angle = fileio->m_pVoxelebXml->sensorxml.viewAngles.size();
    modelio->isTemperature =  fileio->m_pVoxelebXml->sensorxml.isTemperature;
    modelio->isDisplay = fileio->m_pVoxelebXml->sensorxml.isDisplay;
    modelio->isAlbedo = fileio->m_pVoxelebXml->sensorxml.isAlbedo;
    modelio->isImage = fileio->m_pVoxelebXml->sensorxml.isImage;
    modelio->imageSize = fileio->m_pVoxelebXml->sensorxml.resolution;
    modelio->maxDepth = fileio->m_pVoxelebXml->settingxml.maxDepth;
    modelio->n_sample = fileio->m_pVoxelebXml->settingxml.n_sample;

    return true;
}

bool Voxeleb::updateSetting(std::shared_ptr<VoxelebIO> &modelio){

    // auto &opo = modelio->m_opo;

    // auto &sceneio = modelio->m_sceneio;

    modelio->setting.imageSize = modelio->imageSize;
    modelio->setting.n_wave = modelio->n_wave;
    modelio->setting.scale = modelio->stepsize_surface;
   // modelio->setting.isTemperature = modelio->isTemperature;
    modelio->setting.isDisplay = modelio->isDisplay;
    modelio->setting.maxDepth = modelio->maxDepth;
    modelio->setting.n_sample = modelio->n_sample;
    modelio->setting.voxelSize = modelio->voxelSize_XZY;


    return true;
    //modelio->setting.maxDepth = fileio->m_pXmlInput.
}


bool  Voxeleb::uploadMeteo(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelebIO> &modelio){


   // Utils::readascfileinout(meteofile,0,1,)

   fileio->readMeteo(modelio->m_defined,modelio->n_node,modelio->meteos,modelio->atomconds);
   return true;
}

bool  Voxeleb::updateMeteo(std::shared_ptr<VoxelebIO> &modelio, int knode){

    modelio->meteo = modelio->meteos[knode];

    nvvk::CommandPool cmdBufGet(modelio->m_device, modelio->m_queueIndex);
    vk::CommandBuffer cmdBuf = cmdBufGet.createCommandBuffer();
    vkCmdUpdateBuffer(cmdBuf, (*modelio->m_pMeteoBuffer).buffer, 0, sizeof(Meteo), &modelio->meteo);
    cmdBufGet.submitAndWait(cmdBuf);

    return true;
}

bool Voxeleb::uploadDefined(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelebIO> &modelio)
{

    fileio->readDefined(modelio->m_defined);

    return false;
}

bool Voxeleb::create(std::shared_ptr<VoxelebIO> &modelio) {


    m_pBuffer->createBuffer(modelio);
    m_pDescriptor->createDescriptor(modelio);
    m_pPipeline->createPipeline(modelio);
    m_pCommand->create(modelio);
    updateSetting(modelio);
    return true;
}

bool Voxeleb::run(std::shared_ptr<VoxelebIO> &modelio, std::shared_ptr<FileIO> &fileio) {


for(int knode = 20; knode < 36;knode ++) {



    modelio->k_node = knode;

    updateMeteo(modelio,knode);

    m_pCommand->runEB(modelio);

    std::cout << "Time Info:" << "    t_" << std::to_string(modelio->meteo.t) << std::endl;
    outputVoxel(modelio,fileio);
    for (int kangle = 0; kangle < modelio->n_angle; kangle++) {
        modelio->k_angle = kangle;
        m_pGeometry->updateAngle(modelio,kangle);

        Angle angle = modelio->angles[kangle];
        std::cout << "Angle Info:"
                  << "    vza_" << std::to_string(angle.vza) << "    vaa_" << std::to_string(angle.vaa)
                  << "    sza_" << std::to_string(angle.sza) << "    saa_" << std::to_string(angle.saa) << std::endl;

        ////updateSetting(modelio);
        m_pCommand->runRT(modelio);

//        std::cout << "Success: " << kangle << std::endl;

        output(modelio,fileio,knode, kangle);

    }

}
    return true;
}



bool Voxeleb::destroy(std::shared_ptr<VoxelebIO> & modelio){

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



void Voxeleb::output(std::shared_ptr<VoxelebIO> &modelio, std::shared_ptr<FileIO> &fileio, int knode, int kangle) {


    VkBufferUsageFlags usage{VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT};
    int width = modelio->imageSize.x;
    int height = modelio->imageSize.y;
    int n_wave = modelio->n_wave;
    VkDeviceSize bufferSize = width * height *n_wave* sizeof(float);
    nvvk::Buffer pixelBuffer = modelio->m_pAlloc->createBuffer(bufferSize, usage,
                                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    m_pVirtual->bufferToBuffer(modelio, *(modelio->m_virtualio->m_pBufferStorage), bufferSize, pixelBuffer);

    // write the buffer to disk
    void *data = modelio->m_pAlloc->map(pixelBuffer);
    float *pData = reinterpret_cast<float *>(data);

    float test0 = pData[0];
//     float test6 = pData[10000];
//     float test10 = pData[250000];
    //std::cout << "value: " << test0 << std::endl;

    modelio->m_pAlloc->unmap(pixelBuffer);
    modelio->m_pAlloc->destroy(pixelBuffer);

    Angle angle = modelio->angles[kangle];
    std::vector<float> waves = modelio->waves;
    glm::vec2 resolution = modelio->imageSize;



    Eigen::VectorXd cx;
    Eigen::VectorXd cy;
    m_pGeometry->orthcorrect(modelio,angle.vza,angle.vaa,cy,cx);


    float *pData_orth = new float[width*height*n_wave];
    std::memset(pData_orth,0,width*height*n_wave*sizeof(float));
    for(int i=0;i<width;i++)
    {
        for(int j=0;j<height;j++)
        {
            int old = j*width+i;
            int ii,jj;
            ii = int(i*cx[0]+j*cx[1]+i*j*cx[2]+cx[3]);
            jj = int(i*cy[0]+j*cy[1]+i*j*cy[2]+cy[3]);
            int orth = ii*height + jj;

            if (orth <0) continue;
            if(orth > height*width) continue;
            for(int k=0;k<n_wave;k++)
            {
                int oldd = k*width*height + old;

                int orthh =  k*width*height + orth;
                if (pData[orthh]==0) continue;
                pData_orth[oldd] = pData[orthh];
            }
        }
    }



    float t = modelio->meteo.t;
    fileio->writeENVIdata(modelio->projectDir, pData_orth, width, height, n_wave, angle, t);

    // fileio


    //m_pFileOutput->writeTif(, pData, angles, waves, resolution);
}

void Voxeleb::outputVoxel(std::shared_ptr<VoxelebIO> &modelio, std::shared_ptr<FileIO> &fileio) {


//    VkBufferUsageFlags usage{VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT};
//    int num = modelio->n_voxel;
//    VkDeviceSize bufferSize = num * sizeof(VoxelHeatflux);
//    nvvk::Buffer pixelBuffer = modelio->m_pAlloc->createBuffer(bufferSize, usage,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
//    m_pVirtual->bufferToBuffer(modelio, *(modelio->m_voxelio->m_pFluxBuffer), bufferSize, pixelBuffer);
//    // write the buffer to disk
//    void *data = modelio->m_pAlloc->map(pixelBuffer);
//    VoxelHeatflux *pData = reinterpret_cast<VoxelHeatflux *>(data);
//    VoxelHeatflux test0 = pData[0];
//    modelio->m_pAlloc->unmap(pixelBuffer);
//    modelio->m_pAlloc->destroy(pixelBuffer);


//    VkBufferUsageFlags usage{VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT};
//    int num = modelio->n_voxel;
//    VkDeviceSize bufferSize = num * sizeof(VoxelRss);
//    nvvk::Buffer pixelBuffer = modelio->m_pAlloc->createBuffer(bufferSize, usage,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
//    m_pVirtual->bufferToBuffer(modelio, *(modelio->m_voxelio->m_pRssBuffer), bufferSize, pixelBuffer);
//    // write the buffer to disk
//    void *data = modelio->m_pAlloc->map(pixelBuffer);
//    VoxelRss *pData = reinterpret_cast<VoxelRss *>(data);
//    VoxelRss test0 = pData[0];
//    modelio->m_pAlloc->unmap(pixelBuffer);
//    modelio->m_pAlloc->destroy(pixelBuffer);


//    VkBufferUsageFlags usage{VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT};
//    int num = modelio->n_voxel;
//    VkDeviceSize bufferSize = num * sizeof(VoxelTempe);
//    nvvk::Buffer pixelBuffer = modelio->m_pAlloc->createBuffer(bufferSize, usage,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
//    m_pVirtual->bufferToBuffer(modelio, *(modelio->m_voxelio->m_pTempeBuffer), bufferSize, pixelBuffer);
//    // write the buffer to disk
//    void *data = modelio->m_pAlloc->map(pixelBuffer);
//    VoxelTempe *pData = reinterpret_cast<VoxelTempe *>(data);
//    VoxelTempe test0 = pData[0];
//    modelio->m_pAlloc->unmap(pixelBuffer);
//    modelio->m_pAlloc->destroy(pixelBuffer);

}


bool Voxeleb::uploadAero(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelebIO> &modelio) {


    if(fileio->m_pVoxelebXml->aerocondxml.aerotype == AeroType::ONE) {
        modelio->aeroconds.emplace_back(fileio->m_pVoxelebXml->aerocondxml.aerocond);
    }else if (fileio->m_pVoxelebXml->aerocondxml.aerotype == AeroType::image)
    {
        int a = 10;
    }else if(fileio->m_pVoxelebXml->aerocondxml.aerotype == AeroType::gridCal){
        int b = 10;
    }

    return false;


}

