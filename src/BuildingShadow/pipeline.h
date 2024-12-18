//
// Created by ShIP on 2024/12/2.
//

#ifndef FIELD_PIPELINE_H
#define FIELD_PIPELINE_H


#include <nvvk/pipeline_vk.hpp>
#include <nvvk/sbtwrapper_vk.hpp>
#include <nvvk/shadermodulemanager_vk.hpp>
#include <nvvk/shaders_vk.hpp>
#include <nvh/shaderfilemanager.hpp>
#include <nvh/fileoperations.hpp>
#include <nvh/alignment.hpp>
#include <future>

#include "buildingshadowio.h"



class Pipeline {

public:
    Pipeline(){};

    bool createPipeline(std::shared_ptr<BuildingShadowIO> &modelio);
    void createShaderBindingTable(std::shared_ptr<BuildingShadowIO> &modelio);
    void destroy(std::shared_ptr<BuildingShadowIO> &modelio);

};



#endif //PIPELINE_H
