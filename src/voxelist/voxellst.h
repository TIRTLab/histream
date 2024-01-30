//
// Created by admin on 2024/1/26.
//

#ifndef FIELD_VOXELLST_H
#define FIELD_VOXELLST_H

#include <string>
#include <vector>
#include <map>
#include "src/voxelist/voxellstio.h"
#include "src/base/structs.h"
#include "src/base/geometry.h"
#include "src/base/compo.h"
#include "src/base/scene.h"
#include "src/raytracing/pipeline.h"
#include "src/base/virtualscreen.h"
#include "src/raytracing/buffer.h"
#include "src/base/accelstruct.h"
#include "src/raytracing/descriptor.h"
#include "src/raytracing/command.h"
#include "src/base/utils.h"

class Voxellst {
public:
    Voxellst(){
        m_pGeometry = std::make_shared<Geometry>();
        m_pScene = std::make_shared<Scene>();
        m_pCompo = std::make_shared<Compo>();

        m_pPipeline = std::make_shared<Pipeline>();
        m_pVirtual  = std::make_shared<VirtualScreen>();
        m_pBuffer = std::make_shared<Buffer>();
        m_pDescriptor = std::make_shared<Descriptor>();
        m_pCommand = std::make_shared<Command>();
    };


    bool setup(AppSetting &appsetting, std::shared_ptr<VoxellstIO> &voxellstio);
    bool upload(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &voxellstio);


    bool updateSetting(std::shared_ptr<VoxellstIO> &voxellstio);
    bool uploadSetting(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &voxellstio);


    std::shared_ptr<Geometry> m_pGeometry;
    std::shared_ptr<Scene> m_pScene;
    std::shared_ptr<Compo> m_pCompo;
    std::shared_ptr<Pipeline> m_pPipeline;
    std::shared_ptr<VirtualScreen> m_pVirtual;
    std::shared_ptr<Buffer>  m_pBuffer;
    std::shared_ptr<Descriptor> m_pDescriptor;
    std::shared_ptr<Command> m_pCommand;

};


#endif //FIELD_VOXELLST_H
