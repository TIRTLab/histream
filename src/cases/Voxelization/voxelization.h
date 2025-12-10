//
// Created by admin on 8/12/2025.
//

#ifndef STREAM_VK_VOXELIZATION_H
#define STREAM_VK_VOXELIZATION_H
#include <string>
#include <vector>
#include <map>
#include "voxelizationio.h"
#include "src/base/structs.h"
#include "src/base/geometry.h"
#include "src/base/compo.h"
#include "src/base/scene.h"
#include "src/base/virtualscreen.h"
#include "src/base/accelstruct.h"
#include "src/base/utils.h"

#include "src/cases/Voxelization/buffer_voxelization.h"
#include "src/cases/Voxelization/command_voxelization.h"
#include "src/cases/Voxelization/descriptor_voxelization.h"
#include "src/cases/Voxelization/pipeline_voxelization.h"


class Voxelization{
public:
    Voxelization() {
        m_pBuffer = std::make_shared<Buffer_voxelization>();
        m_pDescriptor = std::make_shared<Descriptor_voxelization>();
        m_pPipeline = std::make_shared<Pipeline_voxelization>();
        m_pCommand = std::make_shared<Command_voxelization>();

        m_pCompo = std::make_shared<Compo>();
        m_pScene = std::make_shared<Scene>(); // 复用 Scene 类来加载模型
        m_pGeometry = std::make_shared<Geometry>();
    };

    // [修改] 参数改为 AppSetting 和 FileIO，不再依赖 RaytracingIO
    bool setup(AppSetting &appSetting, std::shared_ptr<VoxelizationIO> &modelio);
    bool upload(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelizationIO> &modelio);

    bool create(std::shared_ptr<VoxelizationIO> &modelio);
    bool run(std::shared_ptr<VoxelizationIO> &modelio);
    void destroy(std::shared_ptr<VoxelizationIO> &modelio);
    void debugOutput(std::shared_ptr<VoxelizationIO>& modelio);

private:
    std::shared_ptr<Buffer_voxelization>     m_pBuffer;
    std::shared_ptr<Descriptor_voxelization> m_pDescriptor;
    std::shared_ptr<Pipeline_voxelization>   m_pPipeline;
    std::shared_ptr<Command_voxelization>    m_pCommand;
    std::shared_ptr<Compo>           m_pCompo;
    std::shared_ptr<Scene>           m_pScene;
    std::shared_ptr<Geometry>        m_pGeometry;
};


#endif //STREAM_VK_VOXELIZATION_H
