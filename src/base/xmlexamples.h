//
// Created by admin on 2024/1/26.
//

#ifndef FIELD_RAYTRACINGXMLEXAMPLE_H
#define FIELD_RAYTRACINGXMLEXAMPLE_H

#include "structs.h"
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <numbers>
#include <algorithm>
#include <vector>


class XmlExamples{

public:
    XmlExamples(){
        createRaytracingxml();
    }
    std::shared_ptr<RaytracingXml> m_pRaytracingXml;
    std::shared_ptr<VoxelLstXml>   m_pVoxelLstXml;

    void createRaytracingxml();
    void createVoxellstxml();
};





#endif //FIELD_RAYTRACINGXMLEXAMPLE_H
