//
// Created by admin on 2024/1/24.
//

#ifndef FIELD_SCENE_H
#define FIELD_SCENE_H

#include "src/base/fileio.h"
#include "src/base/meshio.h"
#include "structs.h"
#include "src/base/objloader.h"
#include "src/raytracing/raytracingio.h"
#include "src/voxelist/voxellstio.h"
#include "src/thirdparty/NanoVDB.h"
#include "src/thirdparty/nanoutil/GridBuilder.h"
#include "src/thirdparty/nanoutil/Primitives.h"
#include "src/base/voxeldesigner.h"



class Scene {
public:
    Scene(){};

    bool createObjScene(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &raytracingio);
   //bool createPrimScene(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &raytracingio);
   PrimMesh XYZ2XZY(PrimMesh model);
   ObjMesh XYZ2XZY(ObjMesh model);
   bool createPrimScene(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &voxellstio);
};


#endif //FIELD_SCENE_H
