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
        createVoxelebxml();
     //   createEcolstxml();
    }
    std::shared_ptr<RaytracingXml> m_pRaytracingXml;
    std::shared_ptr<VoxelEBXml>   m_pVoxelebXml;
    //std::shared_ptr<Huailai::VoxelEBXml> m_pHuailaiXml;

    void createRaytracingxml();
    void createVoxelebxml();

    //void createEcolstxml();
};





#endif //FIELD_RAYTRACINGXMLEXAMPLE_H
