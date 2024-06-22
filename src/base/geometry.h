//
// Created by admin on 2024/1/24.
//

#ifndef FIELD_GEOMETRY_H
#define FIELD_GEOMETRY_H

#include "src/raytracing/raytracingio.h"
#include "src/voxeleb/voxelebio.h"
#include "nvh/cameramanipulator.hpp"
#include "nvmath/nvmath.h"
#include "utils.h"
#include "thirdparty/spa.h"
#include <Eigen/Dense>

class Geometry {
public:
    Geometry(){};

    bool createGeometry(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &modelio);
    bool createGeometry(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelebIO> &modelio);
    SensorMatrix createSensor(glm::vec3 size, glm::vec3 origin, float vza, float vaa, float ratio = 1.0);
    LightSet createLight(float sza, float saa, float direct, float diffuse,float,float);

    void updateSensor(std::shared_ptr<RaytracingIO> &modelio, SensorMatrix &sensor);
    void updateLight(std::shared_ptr<RaytracingIO> &modelio, LightSet &light);
    void updateSensor(std::shared_ptr<VoxelebIO> &modelio, SensorMatrix &sensor);
    void updateLight(std::shared_ptr<VoxelebIO> &modelio, LightSet &light);

    static void updateSolarAngle(std::shared_ptr<VoxelebIO> &modelio, Angle &angle);

    void updateAngle(std::shared_ptr<RaytracingIO> &modelio, int kangle);
    void updateAngle(std::shared_ptr<VoxelebIO> &modelio, int kangle);


    void orthcorrect(std::shared_ptr<RaytracingIO> &modelio,float vza, float vaa,
                     Eigen::VectorXd & cx, Eigen::VectorXd & cy);

    void orthcorrect(std::shared_ptr<VoxelebIO> &modelio,float vza, float vaa,
                               Eigen::VectorXd &cx, Eigen::VectorXd &cy);

};


#endif //FIELD_GEOMETRY_H
