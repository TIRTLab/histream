//
// Created by admin on 2024/1/26.
//

#include "voxellst.h"



bool Voxellst::setup( AppSetting &appsetting, std::shared_ptr<VoxellstIO> &modelio){


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




bool Voxellst::upload(std::shared_ptr<FileIO> &fileio,std::shared_ptr<VoxellstIO> &modelio){

    // auto & fileio = modelio->m_fileio;
    // auto & meshio = modelio->m_meshio;


    m_pCompo->createCompProperty(fileio, modelio);
    m_pScene->createPrimScene(fileio,modelio);
    m_pGeometry->createGeometry(fileio,modelio);
//    defineOPO(modelio);
    uploadDefined(fileio,modelio);
    uploadMeteo(fileio,modelio);
    uploadSetting(fileio, modelio);

    return true;
}

bool Voxellst::uploadSetting(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &modelio) {

    modelio->n_wave = fileio->m_pVoxelLstXml->sensorxml.waves.size();
    modelio->n_angle = fileio->m_pVoxelLstXml->sensorxml.viewAngles.size();
    modelio->isTemperature =  fileio->m_pVoxelLstXml->sensorxml.isTemperature;
    modelio->isDisplay = fileio->m_pVoxelLstXml->sensorxml.isDisplay;
    modelio->isAlbedo = fileio->m_pVoxelLstXml->sensorxml.isAlbedo;
    modelio->isImage = fileio->m_pVoxelLstXml->sensorxml.isImage;
    modelio->resolution = fileio->m_pVoxelLstXml->sensorxml.resolution;
    modelio->maxDepth = fileio->m_pVoxelLstXml->settingxml.maxDepth;
    modelio->n_sample = fileio->m_pVoxelLstXml->settingxml.n_sample;

    return true;
}

bool Voxellst::updateSetting(std::shared_ptr<VoxellstIO> &modelio){

    // auto &opo = modelio->m_opo;

    // auto &sceneio = modelio->m_sceneio;
    modelio->setting.resolution = modelio->resolution;
    modelio->setting.n_wave = modelio->n_wave;
   // modelio->setting.isTemperature = modelio->isTemperature;
    modelio->setting.isDisplay = modelio->isDisplay;
    modelio->setting.maxDepth = modelio->maxDepth;
    modelio->setting.n_sample = modelio->n_sample;

    return true;
    //modelio->setting.maxDepth = fileio->m_pXmlInput.
}


bool  Voxellst::uploadMeteo(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &modelio){

    auto & meteofile = fileio->m_pVoxelLstXml->meteoxml.meteofile;

    int n_node = 0;


    modelio->aeroconds.emplace_back(fileio->m_pVoxelLstXml->aerocondxml.aerocond);

    std::ifstream infile(meteofile);
    std::vector<std::string> fields;
    std::string deli(" "), line;

    std::getline(infile, line);
    fields = Utils::splitt(line, deli);
    n_node = std::stoi(fields[0].c_str());

    for (int i = 0; i < n_node; i++)
    {
        std::getline(infile, line);
        fields = Utils::splitt(line, deli);

        Meteo mi;
        mi.t = std::atof(fields[0].c_str());
        mi.Ta = std::atof(fields[1].c_str());
        mi.ea = std::atof(fields[2].c_str());
        mi.p = std::atof(fields[3].c_str());
        mi.u = std::atof(fields[4].c_str());
        mi.Rin = std::atof(fields[5].c_str());
        mi.Rli = std::atof(fields[6].c_str());
//        mi.sm = sm;
//        mi.z = z;
//        mi.ea = ea;
//        mi.Ca = Ca;
//        mi.Oa = Oa;
//        mi.Tsold = Tsold;
//        mi.SatWater = SatWater;
//        mi.dTime = dTime;

        modelio->meteos.emplace_back(mi);
        //m_meteoParams.emplace_back(mi);
    }

    modelio->meta = fileio->m_pVoxelLstXml->meteoxml.meta;

    modelio->wavesets.resize(N1+N2);
    for (int i = 0; i <  2001 ; i++)
    {
        modelio->wavesets[i].wavelength = 400 + i;
    }
    for (int i = 0; i < 126; i++)
    {
        modelio->wavesets[i+2001].wavelength = 2500 + i * 100;
    }
    for (int i = 0; i < 35; i++)
    {
        modelio->wavesets[i+2127].wavelength = 16000 + i * 1000;
    }


    // atomCond;
    float  *esun_, *esky_, *fesky_, *fesun_;
    int num = 1;
    // wave_ = Utils::infile2num(predifineDir+'Esk', 0, 0, num);
    esun_ = Utils::readascfile(fileio->m_pVoxelLstXml->meteoxml.rinfile, 0, 0, num);
    esky_ = Utils::readascfile(fileio->m_pVoxelLstXml->meteoxml.rlifile, 0, 0, num);
    fesky_ = new float[num];
    fesun_ = new float[num];

    float TsEsky = 0, TlEsky = 0, TlEsun = 0, TsEsun = 0, tstot = 0, tltot = 0, temp1, temp2, step;
    int b1 = N1;
    int b2 = N1+N2;

    // �̲�
    for (int i = 0; i < b1 - 1; i++)
    {
        temp1 = (esky_[i] + esky_[i + 1]) / 2.0;
        step =  modelio->wavesets[i+1].wavelength -  modelio->wavesets[i].wavelength;
        temp2 = (esun_[i] + esun_[i + 1]) / 2.0;
        TsEsky += temp1 * step;
        TsEsun += temp2 * step;
    }
    tstot = (TsEsky + TsEsun) * 0.001;
    for (int i = 0; i < b1; i++)
    {
        fesky_[i] = esky_[i] / tstot;
        fesun_[i] = esun_[i] / tstot;
    }
    // ����
    for (int j = b1; j < b2 - 1; j++)
    {
        temp1 = (esky_[j] + esky_[j + 1]) / 2.0;
        step =   modelio->wavesets[j+1].wavelength -  modelio->wavesets[j].wavelength;
        temp2 = (esun_[j] + esun_[j + 1]) / 2.0;
        TlEsky += temp1 * step;
        TlEsun += temp2 * step;
    }
    tltot = (TlEsky + TlEsun) * 0.001;
    for (int i = b1; i < b2; i++)
    {
        fesky_[i] = esky_[i] / tltot;
        fesun_[i] = esun_[i] / tltot;
    }

    for (int i = 0; i < b2; i++)
    {
        modelio->wavesets[i].direct = fesun_[i];
        modelio->wavesets[i].diffuse = fesky_[i];
    }



    delete[] fesky_;
    delete[] fesun_;
    delete[] esun_;
    delete[] esky_;

   // Utils::readascfileinout(meteofile,0,1,)

   return true;
}
bool  Voxellst::updateMeteo(std::shared_ptr<VoxellstIO> &modelio){

    return true;
}

bool Voxellst::uploadDefined(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &modelio)
{
    std::string line;
    std::vector<std::string> fields;
    std::string definedpath = fileio->m_pVoxelLstXml->definedDir + "/defined/defined.txt";
    std::string deli(" ");
    std::ifstream infile(definedpath.c_str());
    auto & canopy = modelio->m_defined->canopy;
    auto & leafbio = modelio->m_defined->leafbio;
    auto & soilset = modelio->m_defined->soilset;
    //auto & m_aerocoeff = modelio->m_defined->aerocoeffs[0];
    AeroCoeff m_aerocoeff;
    auto & meta = modelio->m_defined->meta;

    if(infile.is_open())
    {
        getline(infile,line);
        getline(infile,line);
        fields = Utils::splitt(line,deli);
        canopy.lai = std::atof(fields[0].c_str());
        canopy.density = std::atof(fields[1].c_str());
        canopy.height = std::atof(fields[2].c_str());
        canopy.width = std::atof(fields[3].c_str());
        canopy.G = std::atof(fields[4].c_str());
        canopy.LIDFa = std::atof(fields[5].c_str());
        canopy.LIDFb = std::atof(fields[6].c_str());
        canopy.hspot = std::atof(fields[7].c_str());
        canopy.leafwidth = std::atof(fields[8].c_str());
//        canopy.type = std::atof(fields[9].c_str());
//        canopy.dist = std::atof(fields[10].c_str());

        getline(infile,line);
        getline(infile,line);
        fields = Utils::splitt(line,deli);
//        leafbio.fp.Cab = std::atof(fields[0].c_str());
//        leafbio.fp.Cw = std::atof(fields[1].c_str());
//        leafbio.fp.Cdm = std::atof(fields[2].c_str());
//        leafbio.fp.Cs = std::atof(fields[3].c_str());
//        leafbio.fp.N = std::atof(fields[4].c_str());
//        spectral.leafRefl_ir = std::atof(fields[5].c_str());
//        spectral.leafTran_ir = std::atof(fields[6].c_str());

        getline(infile,line);
        getline(infile,line);
        fields = Utils::splitt(line,deli);
//        leafbio.Fqe[0] = std::atof(fields[0].c_str());
//        leafbio.Fqe[1] = std::atof(fields[1].c_str());
        leafbio.Vcmax = std::atof(fields[0].c_str());
        leafbio.m = std::atof(fields[1].c_str());
        leafbio.Type = std::atof(fields[2].c_str());
        leafbio.Tparam[0] = std::atof(fields[3].c_str());
        leafbio.Tparam[1] = std::atof(fields[4].c_str());
        leafbio.Tparam[2] = std::atof(fields[5].c_str());
        leafbio.Tparam[3] = std::atof(fields[6].c_str());
        leafbio.Tparam[4] = std::atof(fields[7].c_str());
        leafbio.Rdparam = std::atof(fields[8].c_str());
        leafbio.Tyear = std::atof(fields[9].c_str());
        leafbio.beta = std::atof(fields[10].c_str());
        leafbio.kNPQs = std::atof(fields[11].c_str());
        leafbio.qLs = std::atof(fields[12].c_str());
        leafbio.kV = std::atof(fields[13].c_str());
        leafbio.stressfactor = std::atof(fields[14].c_str());

        getline(infile,line);
        getline(infile,line);
        fields = Utils::splitt(line,deli);
//        soilset.bsm.BSMBrightness = std::atof(fields[0].c_str());
//        soilset.bsm.BSMlat = std::atof(fields[1].c_str());
//        soilset.bsm.BSMlon = std::atof(fields[2].c_str());
//        spectral.soilRefl_ir = std::atof(fields[3].c_str());
        getline(infile,line);
        getline(infile,line);
        fields = Utils::splitt(line,deli);
        soilset.rss = std::atof(fields[0].c_str());
        soilset.cs = std::atof(fields[1].c_str());
        soilset.rhos = std::atof(fields[2].c_str());
        soilset.lambdas = std::atof(fields[3].c_str());
        soilset.rbs = std::atof(fields[4].c_str());
        soilset.SMC = std::atof(fields[5].c_str());
        soilset.Tsoil = std::atof(fields[6].c_str());
//        soilset.satwater = std::atof(fields[7].c_str());

        getline(infile,line);
        getline(infile,line);
        fields = Utils::splitt(line,deli);
        m_aerocoeff.zo = std::atof(fields[0].c_str());
        m_aerocoeff.d = std::atof(fields[1].c_str());
        m_aerocoeff.rbc = std::atof(fields[2].c_str());
        m_aerocoeff.CR = std::atof(fields[3].c_str());
        m_aerocoeff.Cd = std::atof(fields[4].c_str());
        m_aerocoeff.CD1 = std::atof(fields[5].c_str());
        m_aerocoeff.Psicor = std::atof(fields[6].c_str());
        m_aerocoeff.CSSOIL = std::atof(fields[7].c_str());
        m_aerocoeff.rwc = std::atof(fields[8].c_str());
        m_aerocoeff.rbs = std::atof(fields[9].c_str());
        modelio->m_defined->aerocoeffs.emplace_back(m_aerocoeff);

        getline(infile,line);
        getline(infile,line);
        fields = Utils::splitt(line,deli);
        meta.z = atof(fields[0].c_str());
        meta.sm = atof(fields[1].c_str());
        meta.ea = atof(fields[2].c_str());
        meta.Ca = atof(fields[3].c_str());
        meta.Oa = atof(fields[4].c_str());
        meta.Tsold = atof(fields[5].c_str());
        meta.SatWater = atof(fields[6].c_str());
        meta.dTime = atof(fields[7].c_str());


        //std::cout<<"FileInput has been read..."<<std::endl;

    }else std::cout<<"Unable to open the fileinput "<<std::endl;
    infile.close();

    return false;
}

bool Voxellst::create(std::shared_ptr<VoxellstIO> &modelio) {


    m_pBuffer->createBuffer(modelio);
    m_pDescriptor->createDescriptor(modelio);
    m_pPipeline->createPipeline(modelio);
    m_pCommand->create(modelio);
    updateSetting(modelio);
    return true;
}

bool Voxellst::run(std::shared_ptr<VoxellstIO> &modelio, std::shared_ptr<FileIO> &fileio) {

    for(int kangle = 0; kangle < modelio->n_angle; kangle++)
    {

//        glm::vec4 angles = modelio->angles[kangle];
//        std::cout << "Angle Info:"
//                  << "    vza_" << std::to_string(angles.x) << "    vaa_" << std::to_string(angles.y)
//                  << "    sza_" << std::to_string(angles.z) << "    saa_" << std::to_string(angles.w) << std::endl;
//
//        float ratio = 1.0;
//        //ratio = 0.707;
//        SensorMatrix sensorMatrix = m_pGeometry->createSensor(modelio->sceneSize,modelio->sceneOrigin,
//                                                              angles.x, angles.y, ratio);
//        m_pGeometry->updateSensor(modelio, sensorMatrix);
//
//        LightSet lightSet = m_pGeometry->createLight(angles.z, angles.w,modelio->light.direct,
//                                                     modelio->light.diffuse,modelio->light.solarTemperature,
//                                                     modelio->light.skyTemperature);
//        m_pGeometry->updateLight(modelio,lightSet);
//
//        updateSetting(modelio);
//
//        m_pCommand->run(modelio);
//
//        // output
//        output(modelio,fileio,kangle);

        std::cout << "Success: " << kangle << std::endl;
    }


    return true;
}





