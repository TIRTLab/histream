//
// Created by admin on 8/12/2025.
//

#ifndef STREAM_VK_DESCRIPTOR_H
#define STREAM_VK_DESCRIPTOR_H

#include "voxelizationio.h"

class Descriptor {
public:

    enum BindingInd
    {
        spectral,
        thermal,
        modelLink,
        instanceLink,
        tlas,
        sensor,
        light,
        wave,
        storage
    };


    bool create(std::shared_ptr<VoxelizationIO> &modelio);
    void destroy(std::shared_ptr<VoxelizationIO> &modelio);

};
#endif //STREAM_VK_DESCRIPTOR_H
