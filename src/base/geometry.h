//
// Created by admin on 2024/1/24.
//

#ifndef FIELD_GEOMETRY_H
#define FIELD_GEOMETRY_H

#include "src/raytracing/raytracingio.h"
#include "src/voxellst/voxellstio.h"
#include "nvh/cameramanipulator.hpp"
#include "nvmath/nvmath.h"

class Geometry {
public:
    Geometry(){};

    bool createGeometry(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &modelio);
    bool createGeometry(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &modelio);
    SensorMatrix createSensor(glm::vec3 scenesize, glm::vec3 sceneorigen, float vza, float vaa, float ratio = 1.0);
    LightSet createLight(float sza, float saa, float direct, float diffuse,float,float);

    void updateSensor(std::shared_ptr<RaytracingIO> &modelio, SensorMatrix &sensor);
    void updateLight(std::shared_ptr<RaytracingIO> &modelio, LightSet &light);
    void updateSensor(std::shared_ptr<VoxellstIO> &modelio, SensorMatrix &sensor);
    void updateLight(std::shared_ptr<VoxellstIO> &modelio, LightSet &light);

    void updateAngle(std::shared_ptr<RaytracingIO> &modelio, int kangle);
    void updateAngle(std::shared_ptr<VoxellstIO> &modelio, int kangle);

};


#endif //FIELD_GEOMETRY_H
