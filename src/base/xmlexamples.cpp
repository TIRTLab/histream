//
// Created by admin on 2024/1/26.
//
#include "xmlexamples.h"

void XmlExamples::createRaytracingxml(){
    m_pRaytracingXml=std::make_shared<RaytracingXml>();

    m_pRaytracingXml->projectDir = "D:\\work\\field_obj_project\\";
    m_pRaytracingXml->shaderDir = "D:\\work\\field\\shader\\RayTracing\\";

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


    m_pRaytracingXml->scenexml.sceneSize={50,50,10};
    m_pRaytracingXml->scenexml.sceneOrigin={0,0,0};
    m_pRaytracingXml->scenexml.sMin={0,0,0};
    m_pRaytracingXml->scenexml.sMax={50,50,10};
    m_pRaytracingXml->scenexml.background = {"soil","K310","natural","soilset"};
    ObjEntity objEntity = ObjEntity{"objTree","D:\\work\\field_obj_project\\speed_tree.obj",
                           {"Leaf1","Trunk","Bough"},{"leaf","soil","leaf"},{"K300","K310","K300"},
                           false,{{10,10,0},{25,25,0},{40,40,0}},{1,1,1},{0,0,0}};
    PrimEntity primEntity;
//    primEntity= PrimEntity{"primTree",{"crown"},{"leaf"},{"K300"},{"crown"},
//                                {ShapeType::ELLIPSOID},{{5,5,5,glm::vec3(0,0,0)}},
//                                {{10,40,0},{40,10,0}},{1,1,1},{0,0,0}};
    m_pRaytracingXml->scenexml.objEntities = {objEntity};
    m_pRaytracingXml->scenexml.primEntities = {primEntity};
    m_pRaytracingXml->scenexml.isDEM = {false};
    m_pRaytracingXml->scenexml.DEMPath = "";
    m_pRaytracingXml->scenexml.demResolution = {10,10};

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


void XmlExamples::createVoxellstxml(){
    m_pVoxelLstXml=std::make_shared<VoxelLstXml>();

    m_pVoxelLstXml->projectDir = "D:\\work\\field_obj_project\\";
    m_pVoxelLstXml->definedDir = "D:\\work\\field\\shader\\RayTracing\\";

    m_pVoxelLstXml->settingxml.n_sample=32;
    m_pVoxelLstXml->settingxml.maxDepth = 32;
    m_pVoxelLstXml->settingxml.theGPU = 0;

    m_pVoxelLstXml->lightxml.name = "Solar";
    m_pVoxelLstXml->lightxml.solarAngle = {25,25};
    m_pVoxelLstXml->lightxml.direct = 0.9;
    m_pVoxelLstXml->lightxml.diffuse = 0.1;
    m_pVoxelLstXml->lightxml.skyTemperature = 250;
    m_pVoxelLstXml->lightxml.solarTemperature = 6000;

    m_pVoxelLstXml->sensorxml.name = "UAV";
    m_pVoxelLstXml->sensorxml.resolution = {1024,1024};
    m_pVoxelLstXml->sensorxml.isImage = true;
    m_pVoxelLstXml->sensorxml.isAlbedo = false;
    m_pVoxelLstXml->sensorxml.isDisplay = false;
    m_pVoxelLstXml->sensorxml.isTemperature = true;
    m_pVoxelLstXml->sensorxml.viewAngles = {{10,10},{45,45}};
    m_pVoxelLstXml->sensorxml.waves = {650,850,10500};
    m_pVoxelLstXml->sensorxml.projection = Projection::PARALLAL;

    m_pVoxelLstXml->scenexml.sceneSize={50,50,10};
    m_pVoxelLstXml->scenexml.sceneOrigin={0,0,0};
    m_pVoxelLstXml->scenexml.sMin={0,0,0};
    m_pVoxelLstXml->scenexml.sMax={50,50,10};
    m_pVoxelLstXml->scenexml.stepsize_surface=1;
    m_pVoxelLstXml->scenexml.stepsize_atmosphere = 1000;


    m_pVoxelLstXml->scenexml.background = {"soil","K310","natural","soilset"};
    ObjEntity objEntity = ObjEntity{"objTree","D:\\work\\field_obj_project\\speed_tree.obj",
                                    {"Leaf1","Trunk","Bough"},{"leaf","soil","leaf"},{"K300","K310","K300"},
                                    false,{{10,10,0},{25,25,0},{40,40,0}},{1,1,1},{0,0,0}};
    PrimEntity primEntity;
    primEntity= PrimEntity{"primTree",{"primCorwn"},{"leaf"},{"K300"},{"crown"},{"leafbio"},{Type::VEGETATION},
                                {ShapeType::ELLIPSOID},{{5,5,5,glm::vec3(0,0,0)}},
                                {{10,40,0},{40,10,0}},{1,1,1},{0,0,0}};
    m_pVoxelLstXml->scenexml.objEntities = {objEntity};
    m_pVoxelLstXml->scenexml.primEntities = {primEntity};
    m_pVoxelLstXml->scenexml.isDEM = {false};
    m_pVoxelLstXml->scenexml.DEMPath = "";
    m_pVoxelLstXml->scenexml.demResolution = {10,10};

    SpectralXml spectralXml1;
    spectralXml1.spectralName="soil";
    spectralXml1.type = spectralType::OTHER;
    spectralXml1.reflectances = {0.23,0.25,0.05};
    spectralXml1.transmittance ={0.23,0.25,0};
    spectralXml1.path = "";
    spectralXml1.tau_tir = 0;
    spectralXml1.refl_tir = 0.05;

    SpectralXml spectralXml2{};
    spectralXml2.spectralName="leaf";
    spectralXml2.type = spectralType::LEAFBIO;
    spectralXml2.reflectances = {0.13,0.15,0.025};
    spectralXml2.transmittance ={0.13,0.15,0};
    spectralXml2.fp = {80,0.009,0.012,0,1.4};
    spectralXml2.tau_tir = 0;
    spectralXml2.refl_tir = 0.05;

    m_pVoxelLstXml->spectralxmls = {spectralXml1,spectralXml2};


    m_pVoxelLstXml->thermalxmls = {{"K310",320,300},{"K300",305,295}};


    PropertyXml propertyXml1 = {"soilset",Type::VEGETATION,
                                LeafBio{25,8,0.01,1,0.6396,0.025,{0.2,0.3,288,313,328},15,0.4,0,1,1,0},
                                SoilSet{0,500,1180,1800,1.55,25,0.01,10,25}};
    PropertyXml propertyXml2 = {"soilset",Type::SOIL,
                                LeafBio{25,8,0.01,1,0.6396,0.025,{0.2,0.3,288,313,328},15,0.4,0,1,1,0},
                                SoilSet{0,500,1180,1800,1.55,25,0.01,10,25}};
    m_pVoxelLstXml->propxmls ={propertyXml1,propertyXml2};

    m_pVoxelLstXml->canopyxmls ={{"crown",{1,10,0.5,-0.35,-0.15,0.2,0.2}}};

    m_pVoxelLstXml->aerocondxmls ={{AeroType::defined,"natural",{10,10,3,0.36}}};

    m_pVoxelLstXml->meteopath ="";
    m_pVoxelLstXml->skypath ="";
    m_pVoxelLstXml->sunpath ="";
}

