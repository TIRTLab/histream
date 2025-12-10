//
// Created by admin on 8/12/2025.
//

#ifndef STREAM_VK_COMMAND_H
#define STREAM_VK_COMMAND_H
#include "voxelizationio.h"

class Command_voxelization {
public:
    Command_voxelization()= default;

    bool create(std::shared_ptr<VoxelizationIO> &modelio);

    bool run(std::shared_ptr<VoxelizationIO> &modelio);

    void destroy(std::shared_ptr<VoxelizationIO> &modelio);

};
#endif //STREAM_VK_COMMAND_H
