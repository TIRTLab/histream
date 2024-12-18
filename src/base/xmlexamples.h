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
#include "utils.h"

class XmlExamples{

public:
    XmlExamples(){
        // createRaytracingxml();
        createVoxelebxml();
        // createVoxelrtxml();
     //   createEcolstxml();
    }
    std::shared_ptr<RaytracingXml> m_pRaytracingXml;
    std::shared_ptr<VoxelEBXml>   m_pVoxelebXml;
    std::shared_ptr<VoxelRTXml>   m_pVoxelrtXml;


    void createRaytracingxml();
    void createVoxelebxml();
    void createVoxelrtxml();

};





#endif //FIELD_RAYTRACINGXMLEXAMPLE_H
