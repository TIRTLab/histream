//
// Created by admin on 8/12/2025.
//

#ifndef STREAM_VK_BUFFER_H
#define STREAM_VK_BUFFER_H
#include <nvvk/buffers_vk.hpp>
#include "voxelizationio.h"


class Buffer_voxelization {
public:
    Buffer_voxelization()= default;
    bool create(std::shared_ptr<VoxelizationIO> &modelio);
    void destroy(std::shared_ptr<VoxelizationIO> &modelio);

};
#endif //STREAM_VK_BUFFER_H
