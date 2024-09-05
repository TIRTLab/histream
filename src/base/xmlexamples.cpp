//
// Created by admin on 2024/1/26.
//
#include "xmlexamples.h"


void XmlExamples::createVoxelebxml(){
    m_pVoxelebXml=std::make_shared<VoxelEBXml>();

    m_pVoxelebXml->projectDir = "F:\\work\\field_aoyunlst\\field_data\\aoyun\\";
    m_pVoxelebXml->definedDir = "F:\\work\\field_aoyunlst\\field\\";

    m_pVoxelebXml->settingxml.n_sample=64;
    m_pVoxelebXml->settingxml.maxDepth = 32;
    m_pVoxelebXml->settingxml.theGPU = 0;


    m_pVoxelebXml->lightxml.name = "Solar";
    m_pVoxelebXml->lightxml.solarAngle = {40, 30};
    m_pVoxelebXml->lightxml.direct = 0.9;
    m_pVoxelebXml->lightxml.diffuse = 0.1;
    m_pVoxelebXml->lightxml.skyTemperature = 250;
    m_pVoxelebXml->lightxml.solarTemperature = 6000;

    m_pVoxelebXml->sensorxml.name = "UAV";
    m_pVoxelebXml->sensorxml.resolution = {289*5, 289*5};
    m_pVoxelebXml->sensorxml.isImage = true;
    m_pVoxelebXml->sensorxml.isAlbedo = false;
    m_pVoxelebXml->sensorxml.isDisplay = false;
    m_pVoxelebXml->sensorxml.isTemperature = true;
    m_pVoxelebXml->sensorxml.viewAngles = {{0, 0}};
    m_pVoxelebXml->sensorxml.waves = {10500};
    m_pVoxelebXml->sensorxml.projection = Projection::PARALLAL;

    m_pVoxelebXml->scenexml.background.sceneSize={2890, 2890, 10}; // length, width, height
    m_pVoxelebXml->scenexml.background.sceneOrigin={0, 0, 0};
/*    m_pVoxelebXml->scenexml.background.sMin={0,0,0};
    m_pVoxelebXml->scenexml.background.sMax={600,600,10};*/
    m_pVoxelebXml->scenexml.background.stepsize_surface=1;
    m_pVoxelebXml->scenexml.background.stepsize_height=1;
//    m_pVoxelebXml->scenexml.stepsize_atmosphere = 1000;


    m_pVoxelebXml->scenexml.background.bgSpectralName = "soil";
    m_pVoxelebXml->scenexml.background.bgThermalName = "K310";
    m_pVoxelebXml->scenexml.background.bgPropName = "soilset";
    m_pVoxelebXml->scenexml.background.isDEM = {false};
    m_pVoxelebXml->scenexml.background.DEMFile = "";
    m_pVoxelebXml->scenexml.background.lat = 40;
    m_pVoxelebXml->scenexml.background.lon = 120;


    PrimEntity primEntity1;
    primEntity1= PrimEntity{"building",{"wall","roof"},{"wall","roof"},{"K300","K300"},
                           {"crown","crown"},{"soilset","soilset"},Type::BUILDING,
                           {ShapeType::ELLIPSOID,5,5,5,glm::vec3(0,0,0)},false," ", true,
                           "F:\\work\\field_aoyunlst\\field_data\\aoyun\\beijing_zhong_height_normal_rotation.tif"};

    PrimEntity primEntity2;
    primEntity2 = PrimEntity{"tree",{"crown"},{"leaf"},{"K300"},
                           {"crown"},{"leafbio"},Type::VEGETATION,{ShapeType::ELLIPSOID,10,10,10,glm::vec3(0,0,0)},false,
                           " ", false,
                           "",true,"F:\\work\\field_aoyunlst\\field_data\\aoyun\\entity_0_position.txt"};


    m_pVoxelebXml->scenexml.objEntities = {};
    m_pVoxelebXml->scenexml.primEntities = {primEntity1};



    SpectralXml spectralXml1;
    spectralXml1.spectralName="soil";
    spectralXml1.type = spectralType::OTHER;
    spectralXml1.reflectances = {0.1};
    spectralXml1.transmittance ={0};
    spectralXml1.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_tar_asphalt.txt";
    spectralXml1.tau_tir = 0;
    spectralXml1.refl_tir = 0.05;

    SpectralXml spectralXml3;
    spectralXml3.spectralName="wall";
    spectralXml3.type = spectralType::CUSTOM;
    spectralXml3.reflectances = {0.5};
    spectralXml3.transmittance ={0};
  //  spectralXml3.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_tar_asphalt.txt";
    spectralXml3.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_concrete_cement_solid.txt";
    spectralXml3.tau_tir = 0;
    spectralXml3.refl_tir = 0.05;

    SpectralXml spectralXml4;
    spectralXml4.spectralName="roof";
    spectralXml4.type = spectralType::OTHER;
    spectralXml4.reflectances = {0.5};
    spectralXml4.transmittance ={0};
   // spectralXml4.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_tar_asphalt.txt";
    spectralXml4.path = "f:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_concrete_cement_solid.txt";
    spectralXml4.tau_tir = 0;
    spectralXml4.refl_tir = 0.05;

    SpectralXml spectralXml2{};
    spectralXml2.spectralName="leaf";
    spectralXml2.type = spectralType::PROSPECT;
    spectralXml2.reflectances = {0.025};
    spectralXml2.transmittance ={0};
    spectralXml2.fp = {80,0.009,0.012,0,1.4};
    spectralXml2.tau_tir = 0;
    spectralXml2.refl_tir = 0.05;

    m_pVoxelebXml->spectralxmls = {spectralXml1, spectralXml2, spectralXml3, spectralXml4};


    m_pVoxelebXml->thermalxmls = {{"K310", 320, 300}, {"K300", 305, 295}};


    PropertyXml propertyXml1,propertyXml2,propertyXml3;
    propertyXml1 = {"soilset",Type::SOIL};
    propertyXml1.soilset = SoilSet{1,500,1180,1800,1.55,0.25,25,0.45};

    propertyXml2 = {"leafbio",Type::VEGETATION};
    propertyXml2.leafbio = LeafBio{25,8,0.01,4,0.6396,0.025,{0.2,0.3,288,313,328},25,0.4,0,1,1,0};

    propertyXml3 = {"buildup",Type::BUILDING};
    propertyXml3.buildup = BuildUp{1,500,1180,1800,1.55,25};




    m_pVoxelebXml->propxmls ={propertyXml1, propertyXml2};

    m_pVoxelebXml->canopyxmls ={{"crown", {1, 1, 1, 1, 0.5, -0.35, -0.15, 0.2, 0.2}}};

    m_pVoxelebXml->aerocondxml = {AeroType::ONE, {1, 10, 10, 3, 12.}, "", 1000};

    m_pVoxelebXml->meteoxml.meteofile ="F:\\work\\field_aoyunlst\\field\\defined\\meteo.txt";
    m_pVoxelebXml->atomcondxml.rlifile ="F:\\work\\field_aoyunlst\\field\\defined\\Esky_.dat";
    m_pVoxelebXml->atomcondxml.rinfile ="F:\\work\\field_aoyunlst\\field\\defined\\Esun_.dat";
    //   m_pVoxelebXml->meteoxml.aerocond = {10,10,3,0.36};
}


void XmlExamples::createRaytracingxml(){
    m_pRaytracingXml=std::make_shared<RaytracingXml>();

    m_pRaytracingXml->projectDir = "F:\\work\\field_aoyunlst\\field_data\\aoyun\\";
    m_pRaytracingXml->definedDir = "F:\\work\\field_aoyunlst\\field\\";

    m_pRaytracingXml->settingxml.n_sample=32;
    m_pRaytracingXml->settingxml.maxDepth = 32;
    m_pRaytracingXml->settingxml.theGPU = 0;

    m_pRaytracingXml->sensorxml.name = "UAV";
    m_pRaytracingXml->sensorxml.resolution = {1024,1024};
    m_pRaytracingXml->sensorxml.isImage = true;
    m_pRaytracingXml->sensorxml.isAlbedo = false;
    m_pRaytracingXml->sensorxml.isDisplay = false;
    m_pRaytracingXml->sensorxml.isTemperature = true;
    m_pRaytracingXml->sensorxml.viewAngles = {{10,10},{45,45}};
    m_pRaytracingXml->sensorxml.waves = {650,850,10500};
    m_pRaytracingXml->sensorxml.projection = Projection::PARALLAL;


    m_pRaytracingXml->lightxml.name = "Solar";
    m_pRaytracingXml->lightxml.solarAngle = {25,25};
    m_pRaytracingXml->lightxml.direct = 0.9;
    m_pRaytracingXml->lightxml.diffuse = 0.1;
    m_pRaytracingXml->lightxml.skyTemperature = 250;
    m_pRaytracingXml->lightxml.solarTemperature = 6000;


    m_pRaytracingXml->scenexml.background.sceneSize={5715,5081,10};
    m_pRaytracingXml->scenexml.background.sceneOrigin={0,0,0};
//    m_pRaytracingXml->scenexml.sMin={0,0,0};
//    m_pRaytracingXml->scenexml.sMax={50,50,10};
    m_pRaytracingXml->scenexml.background.bgSpectralName ="soil";
    m_pRaytracingXml->scenexml.background.bgThermalName ="K310";
    m_pRaytracingXml->scenexml.background.bgPropName ="soilset";
    m_pRaytracingXml->scenexml.background.isDEM = {false};
    m_pRaytracingXml->scenexml.background.DEMFile = "";
    m_pRaytracingXml->scenexml.background.stepsize_surface = 1;

    ObjEntity objEntity1 = ObjEntity{"aoyun","F:\\work\\field_aoyun\\field_data\\aoyun\\studyarea.obj",
                                     {"wall","roof"},{Type::BUILDING,Type::BUILDING},{"soil","soil"},{"K310","K310"},
                                     {{2857.5,2540.7,0}},{1},{0},false};

    ObjEntity objEntity2 = ObjEntity{"aoyun","F:\\work\\field_aoyun\\field_data\\aoyun\\single_tree_a2_b3_q0.3_lai2.7.obj",
                                     {"Crown"},{Type::VEGETATION},{"leaf"},{"K300"},
                                     {},{},{},true,"F:\\work\\field_aoyun\\field_data\\aoyun\\entity_1_position.txt"};


    PrimEntity primEntity;
//    primEntity= PrimEntity{"primTree",{"crown"},{"leaf"},{"K300"},{"crown"},
//                                {ShapeType::ELLIPSOID},{{5,5,5,glm::vec3(0,0,0)}},
//                                {{10,40,0},{40,10,0}},{1,1,1},{0,0,0}};
    m_pRaytracingXml->scenexml.objEntities = {objEntity1,objEntity2};
    m_pRaytracingXml->scenexml.primEntities = {primEntity};


    SpectralXml spectralXml1;
    spectralXml1.spectralName="soil";
    spectralXml1.type = spectralType::CUSTOM;
    spectralXml1.reflectances = {0.23,0.25,0.05};
    spectralXml1.transmittance ={0.23,0.25,0};
    SpectralXml spectralXml2{};
    spectralXml2.spectralName="leaf";
    spectralXml2.type = spectralType::CUSTOM;
    spectralXml2.reflectances = {0.13,0.15,0.025};
    spectralXml2.transmittance ={0.13,0.15,0};
    m_pRaytracingXml->spectralxmls = {spectralXml1,spectralXml2};


    m_pRaytracingXml->thermalxmls = {{"K310",320,300},{"K300",305,295}};
}

void XmlExamples::createVoxelrtxml(){
    m_pVoxelrtXml=std::make_shared<VoxelRTXml>();

    m_pVoxelrtXml->projectDir = "F:\\work\\field_aoyunlst\\field_data\\aoyun\\";
    m_pVoxelrtXml->definedDir = "F:\\work\\field_aoyunlst\\field\\";

    m_pVoxelrtXml->settingxml.n_sample=32;
    m_pVoxelrtXml->settingxml.maxDepth = 32;
    m_pVoxelrtXml->settingxml.theGPU = 0;
    m_pVoxelrtXml->settingxml.isUAVtrave = false;


    m_pVoxelrtXml->lightxml.name = "Solar";
    m_pVoxelrtXml->lightxml.solarAngle = {40, 30};
    m_pVoxelrtXml->lightxml.direct = 0.9;
    m_pVoxelrtXml->lightxml.diffuse = 0.1;
    m_pVoxelrtXml->lightxml.skyTemperature = 250;
    m_pVoxelrtXml->lightxml.solarTemperature = 6000;

    m_pVoxelrtXml->sensorxml.name = "UAV";
    m_pVoxelrtXml->sensorxml.resolution = {1024, 1024};
    m_pVoxelrtXml->sensorxml.isImage = true;
    m_pVoxelrtXml->sensorxml.isAlbedo = false;
    m_pVoxelrtXml->sensorxml.isDisplay = false;
    m_pVoxelrtXml->sensorxml.isTemperature = true;
    m_pVoxelrtXml->sensorxml.viewAngles = {{0.5, 0}};
    m_pVoxelrtXml->sensorxml.waves = {10500};
    m_pVoxelrtXml->sensorxml.projection = Projection::PARALLAL;
    m_pVoxelrtXml->sensorxml.uavPoses = {glm::vec3(0,1000,0),glm::vec3(100,1000,0),glm::vec3(200,1000,0)};

    m_pVoxelrtXml->scenexml.background.sceneSize={2888, 2890, 10}; // length, width, height
    m_pVoxelrtXml->scenexml.background.sceneOrigin={0, 0, 0};
/*    m_pVoxelrtXml->scenexml.background.sMin={0,0,0};
    m_pVoxelrtXml->scenexml.background.sMax={600,600,10};*/
    m_pVoxelrtXml->scenexml.background.stepsize_surface=1;
    m_pVoxelrtXml->scenexml.background.stepsize_height=1;
//    m_pVoxelrtXml->scenexml.stepsize_atmosphere = 1000;


    m_pVoxelrtXml->scenexml.background.bgSpectralName = "soil";
    m_pVoxelrtXml->scenexml.background.bgThermalName = "K310";
    m_pVoxelrtXml->scenexml.background.bgPropName = "soilset";
    m_pVoxelrtXml->scenexml.background.isDEM = {false};
    m_pVoxelrtXml->scenexml.background.DEMFile = "";
    m_pVoxelrtXml->scenexml.background.lat = 40;
    m_pVoxelrtXml->scenexml.background.lon = 120;


    PrimEntity primEntity1;
    primEntity1= PrimEntity{"building",{"wall","roof"},{"wall","roof"},{"K300","K300"},
                            {"crown","crown"},{"soilset","soilset"},Type::BUILDING,
                            {ShapeType::ELLIPSOID,5,5,5,glm::vec3(0,0,0)},false," ", true,
                            "F:\\work\\field_aoyunlst\\field_data\\aoyun\\beijing_zhong_height_normal.tif"};

    PrimEntity primEntity2;
    primEntity2 = PrimEntity{"tree",{"crown"},{"leaf"},{"K300"},
                             {"crown"},{"leafbio"},Type::VEGETATION,
                             {ShapeType::ELLIPSOID,10,10,10,glm::vec3(0,0,0)},false,
                             " ", false,
                             "",true,"F:\\work\\field_aoyunlst\\field_data\\aoyun\\entity_0_position.txt"};


    m_pVoxelrtXml->scenexml.objEntities = {};
    m_pVoxelrtXml->scenexml.primEntities = {primEntity1};



    SpectralXml spectralXml1;
    spectralXml1.spectralName="soil";
    spectralXml1.type = spectralType::CUSTOM;
    spectralXml1.reflectances = {0.05};
    spectralXml1.transmittance ={0};
    spectralXml1.path = "F:\\work\\field_aoyunlst\\field\\defined\\soilnew.txt";
    spectralXml1.tau_tir = 0;
    spectralXml1.refl_tir = 0.05;

    SpectralXml spectralXml3;
    spectralXml3.spectralName="wall";
    spectralXml3.type = spectralType::CUSTOM;
    spectralXml3.reflectances = {0.05};
    spectralXml3.transmittance ={0};
    spectralXml3.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_tar_asphalt.txt";
//    spectralXml3.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_concrete_cement_solid.txt";
    spectralXml3.tau_tir = 0;
    spectralXml3.refl_tir = 0.05;

    SpectralXml spectralXml4;
    spectralXml4.spectralName="roof";
    spectralXml4.type = spectralType::CUSTOM;
    spectralXml4.reflectances = {0.05};
    spectralXml4.transmittance ={0};
    spectralXml4.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_tar_asphalt.txt";
//    spectralXml4.path = "D:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_concrete_cement_solid.txt";
    spectralXml4.tau_tir = 0;
    spectralXml4.refl_tir = 0.05;

    SpectralXml spectralXml2{};
    spectralXml2.spectralName="leaf";
    spectralXml2.type = spectralType::CUSTOM;
    spectralXml2.reflectances = {0.025};
    spectralXml2.transmittance ={0};
    spectralXml2.fp = {80,0.009,0.012,0,1.4};
    spectralXml2.tau_tir = 0;
    spectralXml2.refl_tir = 0.05;

    m_pVoxelrtXml->spectralxmls = {spectralXml1, spectralXml2, spectralXml3, spectralXml4};


    m_pVoxelrtXml->thermalxmls = {{"K310", 320, 300}, {"K300", 305, 295}};


//    PropertyXml propertyXml1,propertyXml2,propertyXml3;
//    propertyXml1 = {"soilset",Type::SOIL};
//    propertyXml1.soilset = SoilSet{1,500,1180,1800,1.55,0.25,25,0.45};
//
//    propertyXml2 = {"leafbio",Type::VEGETATION};
//    propertyXml2.leafbio = LeafBio{25,8,0.01,4,0.6396,0.025,{0.2,0.3,288,313,328},25,0.4,0,1,1,0};
//
//    propertyXml3 = {"buildup",Type::BUILDING};
//    propertyXml3.buildup = BuildUp{1,500,1180,1800,1.55,25};
//
//
//    m_pVoxelrtXml->propxmls ={propertyXml1, propertyXml2};

    m_pVoxelrtXml->canopyxmls ={{"crown", {1, 1, 1, 1, 0.5, -0.35, -0.15, 0.2, 0.2}}};

    //m_pVoxelrtXml->canopyxmls ={{"crown", {1, 1, 1, 1, 0.5, -0.35, -0.15, 0.2, 0.2}}};
}



/*
void XmlExamples::createEcolstxml(){
    m_pRaytracingXml=std::make_shared<RaytracingXml>();

    m_pRaytracingXml->projectDir = "D:\\work\\field_aoyun\\field_data\\aoyun\\";
    m_pRaytracingXml->definedDir = "D:\\work\\field_aoyun\\field\\";

    m_pRaytracingXml->settingxml.n_sample=32;
    m_pRaytracingXml->settingxml.maxDepth = 32;
    m_pRaytracingXml->settingxml.theGPU = 0;

    m_pRaytracingXml->sensorxml.name = "UAV";
    m_pRaytracingXml->sensorxml.resolution = {512,512};
    m_pRaytracingXml->sensorxml.isImage = true;
    m_pRaytracingXml->sensorxml.isAlbedo = false;
    m_pRaytracingXml->sensorxml.isDisplay = false;
    m_pRaytracingXml->sensorxml.isTemperature = true;
    m_pRaytracingXml->sensorxml.viewAngles = {{51,1},{45,45}};
    m_pRaytracingXml->sensorxml.waves = {650,850,10500};
    m_pRaytracingXml->sensorxml.projection = Projection::PARALLAL;


    m_pRaytracingXml->lightxml.name = "Solar";
    m_pRaytracingXml->lightxml.solarAngle = {50,25};
    m_pRaytracingXml->lightxml.direct = 0.9;
    m_pRaytracingXml->lightxml.diffuse = 0.1;
    m_pRaytracingXml->lightxml.skyTemperature = 250;
    m_pRaytracingXml->lightxml.solarTemperature = 6000;


    m_pRaytracingXml->scenexml.sceneSize_XYZ={5715,5081,10};
    m_pRaytracingXml->scenexml.sceneOrigin_XYZ={0,0,0};
    m_pRaytracingXml->scenexml.sMin={0,0,0};
    m_pRaytracingXml->scenexml.sMax={5717,5081,10};
    m_pRaytracingXml->scenexml.background = {"streat","kstreat","soilset"};

    ObjEntity objEntity1 = ObjEntity{"aoyun","D:\\work\\field_aoyun\\field_data\\aoyun\\studyarea.obj",
                                    {"wall","roof"},{Type::BUILDING,Type::BUILDING},{"wall","roof"},{"kwall","kroof"},
                                    false,{{2857.5,2540.7,0}},{1},{0}};

    ObjEntity objEntity2 = ObjEntity{"aoyun","D:\\work\\field_aoyun\\field_data\\aoyun\\single_tree_a2_b3_q0.3_lai2.7.obj",
                                    {"Crown"},{Type::VEGETATION},{"leaf"},{"kleaf"},
                                    true,{},{},{},"D:\\work\\field_aoyun\\field_data\\aoyun\\entity_1_position.txt"};


    PrimEntity primEntity;
//    primEntity= PrimEntity{"primTree",{"crown"},{"leaf"},{"K300"},{"crown"},
//                                {ShapeType::ELLIPSOID},{{5,5,5,glm::vec3(0,0,0)}},
//                                {{10,40,0},{40,10,0}},{1,1,1},{0,0,0}};
    m_pRaytracingXml->scenexml.objEntities = {objEntity1,objEntity2};
    m_pRaytracingXml->scenexml.primEntities = {primEntity};
    m_pRaytracingXml->scenexml.isDEM = {false};
    m_pRaytracingXml->scenexml.DEMFile = "";

    SpectralXml spectralXml1;
    spectralXml1.spectralName="streat";
    spectralXml1.type = spectralType::CUSTOM;
    spectralXml1.reflectances = {0.23,0.25,0.05};
    spectralXml1.transmittance ={0.23,0.25,0};

    SpectralXml spectralXml2;
    spectralXml2.spectralName="roof";
    spectralXml2.type = spectralType::CUSTOM;
    spectralXml2.reflectances = {0.23,0.25,0.10};
    spectralXml2.transmittance ={0.23,0.25,0};

    SpectralXml spectralXml3;
    spectralXml3.spectralName="wall";
    spectralXml3.type = spectralType::CUSTOM;
    spectralXml3.reflectances = {0.23,0.25,0.075};
    spectralXml3.transmittance ={0.23,0.25,0};

    SpectralXml spectralXml4{};
    spectralXml4.spectralName="leaf";
    spectralXml4.type = spectralType::CUSTOM;
    spectralXml4.reflectances = {0.13,0.15,0.025};
    spectralXml4.transmittance ={0.13,0.15,0};


    SpectralXml spectralXml5{};
    spectralXml5.spectralName="trunk";
    spectralXml5.type = spectralType::CUSTOM;
    spectralXml5.reflectances = {0.13,0.15,0.025};
    spectralXml5.transmittance ={0.13,0.15,0};
    m_pRaytracingXml->spectralxmls = {spectralXml1,spectralXml2,spectralXml3,spectralXml4,spectralXml5};


    m_pRaytracingXml->thermalxmls = {{"kstreat",320,300},
                                     {"kwall",320,300},
                                     {"kroof",320,300},
                                     {"kleaf",300,295},
                                     };
}


void XmlExamples::createVoxelebxml(){
    m_pVoxelebXml=std::make_shared<VoxelEBXml>();

    m_pVoxelebXml->projectDir = "F:\\work\\field_aoyunlst\\field_data\\aoyun\\";
    m_pVoxelebXml->definedDir = "F:\\work\\field_aoyunlst\\field\\";

    m_pVoxelebXml->settingxml.n_sample=32;
    m_pVoxelebXml->settingxml.maxDepth = 32;
    m_pVoxelebXml->settingxml.theGPU = 0;


    m_pVoxelebXml->lightxml.name = "Solar";
    m_pVoxelebXml->lightxml.solarAngle = {40,30};
    m_pVoxelebXml->lightxml.direct = 0.9;
    m_pVoxelebXml->lightxml.diffuse = 0.1;
    m_pVoxelebXml->lightxml.skyTemperature = 250;
    m_pVoxelebXml->lightxml.solarTemperature = 6000;

    m_pVoxelebXml->sensorxml.name = "UAV";
    m_pVoxelebXml->sensorxml.resolution = {1024,1024};
    m_pVoxelebXml->sensorxml.isImage = true;
    m_pVoxelebXml->sensorxml.isAlbedo = false;
    m_pVoxelebXml->sensorxml.isDisplay = false;
    m_pVoxelebXml->sensorxml.isTemperature = true;
    m_pVoxelebXml->sensorxml.viewAngles = {{0,0},{45,45}};
    m_pVoxelebXml->sensorxml.waves = {10500};
    m_pVoxelebXml->sensorxml.projection = Projection::PARALLAL;

    m_pVoxelebXml->scenexml.sceneSize_XYZ={600,600,50}; // length, width, height
    m_pVoxelebXml->scenexml.sceneOrigin_XYZ={0,0,0};
    m_pVoxelebXml->scenexml.sMin={0,0,0};
    m_pVoxelebXml->scenexml.sMax={600,600,10};
    m_pVoxelebXml->scenexml.stepsize_surface=1;
//    m_pVoxelebXml->scenexml.stepsize_atmosphere = 1000;


    m_pVoxelebXml->scenexml.background = {"soil","K310","soilset"};

    PrimEntity primEntity;
    primEntity= PrimEntity{"building",{"wall","roof"},{"wall","roof"},{"K300","K300"},
                           {"crown","crown"},{"soilset","soilset"},{Type::BUILDING,Type::BUILDING},1,
                           {ShapeType::ELLIPSOID,ShapeType::ELLIPSOID},{{5,5,5,glm::vec3(0,0,0)},{5,5,5,glm::vec3(0,0,0)}},
                           {{90,30,0},},{1,},{0},
                           "F:\\work\\field_aoyun\\field_data\\aoyun\\aoyun_10m_new.tif"};

//    primEntity= PrimEntity{"building",{"roof"},{"soil"},{"K300","K300"},{"crown","crown"},{"soilset","soilset"},{Type::BUILDING,Type::BUILDING},1,
//                           {ShapeType::ELLIPSOID,ShapeType::ELLIPSOID},{{5,5,5,glm::vec3(0,0,0)},{5,5,5,glm::vec3(0,0,0)}},
//                           {{90,30,0},},{1,},{0},
//                           "D:\\work\\field_aoyun\\field_data\\aoyun\\aoyun_10m_new.tif"};

    m_pVoxelebXml->scenexml.objEntities = {};
    m_pVoxelebXml->scenexml.primEntities = {primEntity};
    m_pVoxelebXml->scenexml.isDEM = {false};
    m_pVoxelebXml->scenexml.DEMFile = "";


    SpectralXml spectralXml1;
    spectralXml1.spectralName="soil";
    spectralXml1.type = spectralType::OTHER;
    spectralXml1.reflectances = {0.05};
    spectralXml1.transmittance ={0};
    spectralXml1.path = "F:\\work\\field_aoyunlst\\field\\defined\\soilnew.txt";
    spectralXml1.tau_tir = 0;
    spectralXml1.refl_tir = 0.05;

    SpectralXml spectralXml3;
    spectralXml3.spectralName="wall";
    spectralXml3.type = spectralType::OTHER;
    spectralXml3.reflectances = {0.05};
    spectralXml3.transmittance ={0};
    spectralXml3.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_tar_asphalt.txt";
//    spectralXml3.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_concrete_cement_solid.txt";
    spectralXml3.tau_tir = 0;
    spectralXml3.refl_tir = 0.05;

    SpectralXml spectralXml4;
    spectralXml4.spectralName="roof";
    spectralXml4.type = spectralType::OTHER;
    spectralXml4.reflectances = {0.05};
    spectralXml4.transmittance ={0};
    spectralXml4.path = "F:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_tar_asphalt.txt";
//    spectralXml4.path = "D:\\work\\field_aoyunlst\\field\\defined\\VNIR_construction_concrete_cement_solid.txt";
    spectralXml4.tau_tir = 0;
    spectralXml4.refl_tir = 0.05;

    SpectralXml spectralXml2{};
    spectralXml2.spectralName="leaf";
    spectralXml2.type = spectralType::PROSPECT;
    spectralXml2.reflectances = {0.025};
    spectralXml2.transmittance ={0};
    spectralXml2.fp = {80,0.009,0.012,0,1.4};
    spectralXml2.tau_tir = 0;
    spectralXml2.refl_tir = 0.05;

    m_pVoxelebXml->spectralxmls = {spectralXml1,spectralXml2,spectralXml3,spectralXml4};


    m_pVoxelebXml->thermalxmls = {{"K310",320,300},{"K300",305,295}};


    PropertyXml propertyXml1 = {"soilset",Type::SOIL,
                                LeafBio{25,8,0.01,1,0.6396,0.025,{0.2,0.3,288,313,328},25,0.4,0,1,1,0},
                                SoilSet{1,500,1180,1800,1.55,0.25,25,0.45}};
    PropertyXml propertyXml2 = {"leafbio",Type::VEGETATION,
                                LeafBio{25,8,0.01,1,0.6396,0.025,{0.2,0.3,288,313,328},25,0.4,0,1,1,0},
                                SoilSet{0,500,1180,1800,1.55,0.25,25,0.45}};
    m_pVoxelebXml->propxmls ={propertyXml1,propertyXml2};

    m_pVoxelebXml->canopyxmls ={{"crown",{1,1,1,1,0.5,-0.35,-0.15,0.2,0.2}}};

    m_pVoxelebXml->aerocondxml = {AeroType::ONE, {1, 10, 10, 3, 39.}, "", 1000};

    m_pVoxelebXml->meteoxml.meteofile ="F:\\work\\field_aoyunlst\\field\\defined\\meteo.txt";
    m_pVoxelebXml->meteoxml.rlifile ="F:\\work\\field_aoyunlst\\field\\defined\\Esky_.dat";
    m_pVoxelebXml->meteoxml.rinfile ="F:\\work\\field_aoyunlst\\field\\defined\\Esun_.dat";
    //   m_pVoxelebXml->meteoxml.aerocond = {10,10,3,0.36};
}
*/
