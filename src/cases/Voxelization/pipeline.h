//
// Created by admin on 8/12/2025.
//

#ifndef STREAM_VK_PIPELINE_H
#define STREAM_VK_PIPELINE_H
#include <nvvk/pipeline_vk.hpp>
#include <nvvk/sbtwrapper_vk.hpp>
#include <nvvk/shadermodulemanager_vk.hpp>
#include <nvvk/shaders_vk.hpp>
#include <nvh/shaderfilemanager.hpp>
#include <nvh/fileoperations.hpp>
#include <nvh/alignment.hpp>
#include <future>

#include "voxelizationio.h"



class Pipeline {

public:
    Pipeline(){};

    bool create(std::shared_ptr<VoxelizationIO> &modelio);
    void destroy(std::shared_ptr<VoxelizationIO> &modelio);

};
#endif //STREAM_VK_PIPELINE_H
