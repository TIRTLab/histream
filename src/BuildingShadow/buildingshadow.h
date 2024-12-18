//
// Created by ShIP on 2024/12/2.
//

#ifndef FIELD_BUILDINGSHADOW_H
#define FIELD_BUILDINGSHADOW_H


#include "buildingshadowio.h"
#include "src/base/structs.h"
#include "src/base/geometry.h"
#include "src/base/scene.h"
#include "src/BuildingShadow/pipeline.h"
#include "src/BuildingShadow/buffer.h"
#include "src/BuildingShadow/descriptor.h"
#include "src/BuildingShadow/command.h"
#include "src/base/virtualscreen.h"

class BuildingShadow{
public:
    BuildingShadow()
    {
        m_pGeometry = std::make_shared<Geometry>();
        m_pScene = std::make_shared<Scene>();

        m_pPipeline = std::make_shared<Pipeline>();
        m_pBuffer = std::make_shared<Buffer>();
        m_pDescriptor = std::make_shared<Descriptor>();
        m_pCommand = std::make_shared<Command>();

        m_pVirtual  = std::make_shared<VirtualScreen>();
    }

    std::shared_ptr<Pipeline> m_pPipeline;
    std::shared_ptr<Buffer>  m_pBuffer;
    std::shared_ptr<VirtualScreen> m_pVirtual;
    std::shared_ptr<Descriptor> m_pDescriptor;
    std::shared_ptr<Command> m_pCommand;
    std::shared_ptr<Geometry> m_pGeometry;
    std::shared_ptr<Scene> m_pScene;


    bool setup(AppSetting &appsetting, std::shared_ptr<BuildingShadowIO> &modelio);
    bool upload(std::shared_ptr<FileIO>& fileio, std::shared_ptr<BuildingShadowIO>& modelio);
    bool create(std::shared_ptr<BuildingShadowIO> &modelio);
    bool run(std::shared_ptr<BuildingShadowIO> &modelio, std::shared_ptr<FileIO> &fileio);
    bool destroy( std::shared_ptr<BuildingShadowIO> &modelio);
    void outputFacetInfo(std::shared_ptr<BuildingShadowIO> &modelio, std::shared_ptr<FileIO> &fileio);

};

#endif //BUILDINGSHADOW_H
