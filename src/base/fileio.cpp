//
// Created by admin on 2024/1/24.
//

#include "fileio.h"


bool FileIO::readXml(std::string filePath) {

//    m_mode = Mode::eRaytracing;
//    m_pRaytracingXml = std::move(xmlexamples.m_pRaytracingXml);
    m_mode = Mode::eVoxelLST;
    m_pVoxelLstXml = std::move(xmlexamples.m_pVoxelLstXml);

    return false;

    TiXmlDocument mydoc(filePath.c_str()); // tinyxml.h
    bool isloadOk = mydoc.LoadFile();
    if (!isloadOk)
    {
        std::cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << std::endl;
        exit(1);
    }
    TiXmlElement *RootElement = mydoc.RootElement(); // root node of xml
    TiXmlNode *modeNode = RootElement->FirstChild("Mode");
    TiXmlElement *rayTracingDepthEle = modeNode->FirstChildElement("rendermode");
    Mode  modeInd = (Mode)std::stoi(rayTracingDepthEle->GetText());

    if(modeInd == Mode::eRaytracing)
    {
        m_mode = Mode::eRaytracing;
        m_pRaytracingXml = std::make_shared<RaytracingXml>();
        m_pRaytracingXml->lightxml = readLight(RootElement->FirstChild("Geometry"));
        m_pRaytracingXml->sensorxml = readSensor(RootElement->FirstChild("Geometry"));
        m_pRaytracingXml->spectralxmls = readSpectral(RootElement->FirstChild("Attribute"));
        m_pRaytracingXml->thermalxmls = readThermal(RootElement->FirstChild("Attribute"));
        m_pRaytracingXml->scenexml = readScene(RootElement->FirstChild("Scene"));
        m_pRaytracingXml->settingxml = readSetting(RootElement->FirstChild("Setting"));


    }else if(modeInd == Mode::eVoxelLST){
        m_mode = Mode::eVoxelLST;
        m_pVoxelLstXml = std::make_shared<VoxelLstXml>();

    }
    //readSetting(settingNode);



    return true;


}

//bool FileIO::writeTif(std::string outDir, void *images, glm::vec4 angles, std::vector<float> bands,
//                      glm::vec2 resolution) {
//
//
//    int saveImage(std::string outfilepath, std::vector<float> &c,
//                  int width, int height,int band, std::string proj, double trans[6]);
//
//
//return false;
//}


std::vector<SpectralXml> FileIO::readSpectral(TiXmlNode *node, Mode mode) {

    std::vector<SpectralXml> spectralxmls;


    return spectralxmls;
}


std::vector<ThermalXml> FileIO::readThermal(TiXmlNode *node, Mode mode) {

    std::vector<ThermalXml> thermalxmls;


    return thermalxmls;
}


SensorXml FileIO::readSensor(TiXmlNode *node, Mode mode){
    SensorXml sensorxml;

    return sensorxml;
}
LightXml FileIO::readLight(TiXmlNode *node, Mode mode){
    LightXml lightxml;

    return lightxml;
}

SettingXml FileIO::readSetting(TiXmlNode *node, Mode mode){
    SettingXml settingxml;

    return settingxml;
}

SceneXml FileIO::readScene(TiXmlNode *node, Mode mode){

    SceneXml sceneXml;



    return sceneXml;

}





