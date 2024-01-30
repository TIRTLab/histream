//
// Created by admin on 2024/1/24.
//

#include <iomanip>
#include "raytracing.h"


bool Raytracing::setup( AppSetting &appsetting, std::shared_ptr<RaytracingIO> &raytracingio){


    raytracingio->m_device = appsetting.m_context.m_device;
    raytracingio->m_physicalDevice = appsetting.m_context.m_physicalDevice;
    raytracingio->m_instance = appsetting.m_context.m_instance;
    raytracingio->m_queues = appsetting.m_queues;
    raytracingio->m_queue =  raytracingio->m_queues[eGCT].queue;
    raytracingio->m_queueIndex = raytracingio->m_queues[eGCT].familyIndex;
    //    m_instance = appSetting.m_context.m_instance;
//    m_device = appSetting.m_context.m_device;
//    m_physicalDevice = appSetting.m_context.m_physicalDevice;
//    m_queues = appSetting.m_queues;


//    VkCommandPoolCreateInfo poolCreateInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
//    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//    vkCreateCommandPool(raytracingio->m_device, &poolCreateInfo, nullptr, &raytracingio->m_cmdPool);

    raytracingio->m_genCmdBuf.init(raytracingio->m_device,raytracingio->m_queueIndex);


//    VkPipelineCacheCreateInfo pipelineCacheInfo{ VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
//    vkCreatePipelineCache(raytracingio->m_device, &pipelineCacheInfo, nullptr, &raytracingio->m_pipelineCache);

    raytracingio->m_pAlloc  = std::make_shared<Allocator>();
    raytracingio->m_pAlloc->init(raytracingio->m_instance, raytracingio->m_device, raytracingio->m_physicalDevice);
    raytracingio->m_debug.setup(raytracingio->m_device);

    /// requesting ray tracing properties. from physical device.
    VkPhysicalDeviceProperties2 rayTracingProperties{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
    rayTracingProperties.pNext = &(raytracingio->m_rtProperties);
    vkGetPhysicalDeviceProperties2(raytracingio->m_physicalDevice, &rayTracingProperties);

    if (raytracingio->useSBTWrapper)
    {
        raytracingio->m_sbtWrapper.setup(raytracingio->m_device, raytracingio->m_queueIndex, raytracingio->m_pAlloc.get(), raytracingio->m_rtProperties);
    }

    raytracingio->m_pAccelStruct->m_rtBuilder.setup(raytracingio->m_device, raytracingio->m_pAlloc.get(),raytracingio->m_queueIndex);

    return true;

}




bool Raytracing::upload(std::shared_ptr<FileIO> &fileio,std::shared_ptr<RaytracingIO> &raytracingio){

   // auto & fileio = raytracingio->m_fileio;
   // auto & meshio = raytracingio->m_meshio;


    m_pCompo->createCompOptical(fileio, raytracingio);
    m_pScene->createObjScene(fileio,raytracingio);
    m_pGeometry->createGeometry(fileio,raytracingio);
//    defineOPO(raytracingio);
    uploadSetting(fileio, raytracingio);

    return true;
}

bool Raytracing::create(std::shared_ptr<RaytracingIO> &raytracingio) {


    m_pBuffer->createBuffer(raytracingio);
    m_pDescriptor->createDescriptor(raytracingio);
    m_pPipeline->createPipeline(raytracingio);
    m_pCommand->create(raytracingio);
    updateSetting(raytracingio);
    return true;
}

//bool Raytracing::defineOPO(std::shared_ptr<RaytracingIO> &raytracingio){
//
//    //--------------------------------------------------------
//    //-- BRDF/Temperature
//    //-------------------------------------------------------
////    auto &opo = raytracingio->m_opo;
////    auto &fileio = raytracingio->m_fileio;
//    opo->isTemperature =  fileio->m_pXmlInput->sensor.isTemperature;
//
//    opo->isImage = fileio->m_pXmlInput->sensor.isImageSave;
//    opo->n_sample = fileio->m_pXmlInput->setting.n_sample;
//    opo->maxDepth = fileio->m_pXmlInput->setting.maxDepth;
//    return true;
//
//}

bool Raytracing::uploadSetting(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &raytracingio) {

    raytracingio->n_wave = fileio->m_pRaytracingXml->sensorxml.waves.size();
    raytracingio->n_angle = fileio->m_pRaytracingXml->sensorxml.viewAngles.size();
    raytracingio->isTemperature =  fileio->m_pRaytracingXml->sensorxml.isTemperature;
    raytracingio->isDisplay = fileio->m_pRaytracingXml->sensorxml.isDisplay;
    raytracingio->isAlbedo = fileio->m_pRaytracingXml->sensorxml.isAlbedo;
    raytracingio->isImage = fileio->m_pRaytracingXml->sensorxml.isImage;
    raytracingio->resolution = fileio->m_pRaytracingXml->sensorxml.resolution;
    raytracingio->maxDepth = fileio->m_pRaytracingXml->settingxml.maxDepth;
    raytracingio->n_sample = fileio->m_pRaytracingXml->settingxml.n_sample;

    return true;
}

bool Raytracing::updateSetting(std::shared_ptr<RaytracingIO> &raytracingio){

   // auto &opo = raytracingio->m_opo;

   // auto &sceneio = raytracingio->m_sceneio;
    raytracingio->setting.resolution = raytracingio->resolution;
    raytracingio->setting.n_wave = raytracingio->n_wave;
    raytracingio->setting.isTemperature = raytracingio->isTemperature;
    raytracingio->setting.isDisplay = raytracingio->isDisplay;
    raytracingio->setting.maxDepth = raytracingio->maxDepth;
    raytracingio->setting.n_sample = raytracingio->n_sample;

    return true;
    //raytracingio->setting.maxDepth = fileio->m_pXmlInput.
}


bool Raytracing::run(std::shared_ptr<RaytracingIO> &raytracingio, std::shared_ptr<FileIO> &fileio) {

    for(int kangle = 0; kangle < raytracingio->n_angle; kangle++)
    {

        glm::vec4 angles = raytracingio->angles[kangle];
        std::cout << "Angle Info:"
                  << "    vza_" << std::to_string(angles.x) << "    vaa_" << std::to_string(angles.y)
                  << "    sza_" << std::to_string(angles.z) << "    saa_" << std::to_string(angles.w) << std::endl;

        float ratio = 1.0;
        //ratio = 0.707;
        SensorMatrix sensorMatrix = m_pGeometry->createSensor(raytracingio->sceneSize,raytracingio->sceneOrigin,
                                                              angles.x, angles.y, ratio);
        m_pGeometry->updateSensor(raytracingio, sensorMatrix);

        LightSet lightSet = m_pGeometry->createLight(angles.z, angles.w,raytracingio->light.direct,
                                                     raytracingio->light.diffuse,raytracingio->light.solarTemperature,
                                                     raytracingio->light.skyTemperature);
        m_pGeometry->updateLight(raytracingio,lightSet);

        updateSetting(raytracingio);

        m_pCommand->run(raytracingio);

        // output
        output(raytracingio,fileio,kangle);

        std::cout << "Success: " << kangle << std::endl;
    }


    return true;
}

//bool Raytracing::createGPUBuffer(std::shared_ptr<RaytracingIO> &raytracingio){
//
//return false;
//}



void Raytracing::output(std::shared_ptr<RaytracingIO> &raytracingio,std::shared_ptr<FileIO> &fileio,int kangle) {

    VkBufferUsageFlags usage{VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT};
    int width = raytracingio->resolution.x;
    int height = raytracingio->resolution.y;
    int band = raytracingio->n_wave;
    VkDeviceSize bufferSize = width * height * band * sizeof(float);
    nvvk::Buffer pixelBuffer = raytracingio->m_pAlloc->createBuffer(bufferSize, usage,
                                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    m_pVirtual->bufferToBuffer(raytracingio, *(raytracingio->m_virtualio->m_pBufferStorage), bufferSize, pixelBuffer);

    // write the buffer to disk
    void *data = raytracingio->m_pAlloc->map(pixelBuffer);
    float *pData = reinterpret_cast<float *>(data);
    fileio->outImage.clear();
    for (int kband = 0; kband < band; kband++) {
        uint32_t shift = kband*width*height;
        std::vector<float> outImage1;
        outImage1.assign(pData+shift, pData+shift + width * height);
        fileio->outImage.push_back(outImage1);
    }

     float test0 = pData[0];
//    float test1 = pData[1];
//    float test2 = pData[2];
//    float test3 = pData[3];
//    float test4 = pData[4];
//    float test5 = pData[5];
//    float test6 = pData[6];
    //std::cout << "value: " << test0 << std::endl;

    raytracingio->m_pAlloc->unmap(pixelBuffer);
    raytracingio->m_pAlloc->destroy(pixelBuffer);

    glm::vec4 angles = raytracingio->angles[kangle];
    std::vector<float> waves = raytracingio->waves;
    glm::vec2 resolution = raytracingio->resolution;


    std::ostringstream  oss_x;
    oss_x << std::setw(3)<<std::setfill('0')<<angles.x;
    std::ostringstream  oss_y;
    oss_y << std::setw(3)<<std::setfill('0')<<angles.y;
    std::ostringstream  oss_z;
    oss_z << std::setw(3)<<std::setfill('0')<<angles.z;
    std::ostringstream  oss_w;
    oss_w << std::setw(3)<<std::setfill('0')<<angles.w;

    std::string outPath = raytracingio->projectDir +"/results/VZA=" + oss_x.str() + "_VAA=" + oss_y.str() +
                             "_SZA=" + oss_z.str() + "_SAA=" + oss_w.str()+".tif";

    std::string proj = "";
    double trans[6] ={0,0,0,0,0,0};
    Utils::saveImage(outPath,fileio->outImage,width,height,band,proj,trans);


    std::string tifName = raytracingio->projectDir + +"/results/VZA=" + oss_x.str() + "_VAA=" + oss_y.str() +
                                                     "_SZA=" + oss_z.str() + "_SAA=" + oss_w.str()+".img";
    std::string hdrName = raytracingio->projectDir + +"/results/VZA=" + oss_x.str() + "_VAA=" + oss_y.str() +
                          "_SZA=" + oss_z.str() + "_SAA=" + oss_w.str()+".hdr";

    std::ofstream outfilet1(tifName.c_str(), std::ios::binary);
    outfilet1.write(reinterpret_cast<const char*>(pData), sizeof(float) * width * height * band);
    outfilet1.close();

    std::ofstream outfile(hdrName);
    if (outfile.is_open())
    {
        outfile << "ENVI" << std::endl;
        outfile << "description = {" << std::endl;
        outfile << " File Imported into ENVI.} " << std::endl;
        outfile << "samples = " << width << std::endl;
        outfile << "lines   = " << height << std::endl;
        outfile << "bands   =  " << band << std::endl;
        outfile << "header offset = 0" << std::endl;
        outfile << "file type = ENVI Standard" << std::endl;
        outfile << "data type = 4" << std::endl;
        outfile << "interleave = bip" << std::endl;
        outfile << "sensor type = unknown" << std::endl;
        outfile << "byte order = 0" << std::endl;
        outfile << "wavelength units = Unknown" << std::endl;
        outfile.close();
    }
    outfile.close();

    //m_pFileOutput->writeTif(, pData, angles, waves, resolution);
}

bool Raytracing::destroy( std::shared_ptr<RaytracingIO> &raytracingio)
{

    m_pBuffer->destroy(raytracingio);
    m_pDescriptor->destroy(raytracingio);
    m_pPipeline->destroy(raytracingio);
    m_pCommand->destroy(raytracingio);

    raytracingio->m_sbtWrapper.destroy();
    raytracingio->m_pAccelStruct->m_rtBuilder.destroy();

    vkDeviceWaitIdle(raytracingio->m_device);
    raytracingio->m_genCmdBuf.deinit();
//    vkDestroyPipelineCache(m_device, m_pipelineCache, nullptr);
//    vkDestroyCommandPool(m_device, m_cmdPool, nullptr);
    raytracingio->m_pAlloc->deinit();

    return true;
}
