//
// Created by admin on 2024/1/24.
//

#ifndef FIELD_GEOMETRY_H
#define FIELD_GEOMETRY_H

#include "src/raytracing/raytracingio.h"
#include "src/voxelist/voxellstio.h"
#include "nvh/cameramanipulator.hpp"
#include "nvmath/nvmath.h"

class Geometry {
public:
    Geometry(){};

    bool createGeometry(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &raytracingio);
    bool createGeometry(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &voxellstio);
    SensorMatrix createSensor(glm::vec3 scenesize, glm::vec3 sceneorigen, float vza, float vaa, float ratio = 1.0);
    LightSet createLight(float sza, float saa, float direct, float diffuse,float,float);

    void updateSensor(std::shared_ptr<RaytracingIO> &raytracingio, SensorMatrix &sensor);
    void updateLight(std::shared_ptr<RaytracingIO> &raytracingio, LightSet &light);

};


#endif //FIELD_GEOMETRY_H
