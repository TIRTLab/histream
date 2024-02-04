//
// Created by admin on 2024/1/24.
//

#ifndef FIELD_FILEIO_H
#define FIELD_FILEIO_H

#include "structs.h"
#include "src/thirdparty/tinyxml.h"
#include "gdal.h"
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <numbers>
#include <algorithm>
#include "xmlexamples.h"
#include "src/base/utils.h"
#include "src/base/defined.h"

class FileIO {

public:
    FileIO(){};

    bool readXml(std::string path);
    //bool readRayTracingXML(std::string inputPath){ return true;};

    void writeTif(std::string outDir,void *images,glm::vec4 angles, std::vector<float> bands, glm::vec2 resolution);



public:

    std::shared_ptr<RaytracingXml> m_pRaytracingXml;
    std::shared_ptr<VoxelLstXml>   m_pVoxelLstXml;


    std::vector<SpectralXml> readSpectral(TiXmlNode *node, Mode mode=Mode::eRaytracing);
    std::vector<ThermalXml> readThermal(TiXmlNode *node, Mode mode=Mode::eRaytracing);
    SensorXml readSensor(TiXmlNode *node, Mode mode=Mode::eRaytracing);
    LightXml readLight(TiXmlNode *node, Mode mode=Mode::eRaytracing);
    SettingXml readSetting(TiXmlNode *node, Mode mode=Mode::eRaytracing);
    SceneXml readScene(TiXmlNode *node, Mode mode=Mode::eRaytracing);

    void readDefined(std::shared_ptr<DefinedIO> & defineio);
    void readMeteo(std::shared_ptr<DefinedIO> &defineio,int & n_node, std::vector<Meteo> &meteos, std::vector<WaveSet> &wavesets);

    void writeENVIdata(std::string projectDir,float * pData, int width, int height, int band, Angle &angle, int kband = -1);



    XmlExamples xmlexamples;

    Mode m_mode = Mode::eRaytracing;


    std::vector<float> outImage1;
    std::vector<std::vector<float>> outImage;
};


#endif //FIELD_FILEIO_H
