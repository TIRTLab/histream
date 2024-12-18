//
// Created by admin on 2024/1/25.
//

#include "engine.h"



void Engine::init(Mode mode)
{
    appSetting.init();

    if(mode == Mode::eRaytracing) {
        m_pRaytracing = std::make_shared<Raytracing>();
        m_pRaytracingio = std::make_shared<RaytracingIO>();
    }else if(mode == Mode::eVoxelEB) {
        m_pVoxeleb = std::make_shared<Voxeleb>();
        m_pVoxelebio = std::make_shared<VoxelebIO>();
    }else if(mode == Mode::eVoxelRT) {
        m_pVoxelrt = std::make_shared<Voxelrt>();
        m_pVoxelrtio = std::make_shared<VoxelrtIO>();
    }else if (mode == Mode::eBuildingShadow)
    {
        m_pBuildingShadow = std::make_shared<BuildingShadow>();
        m_pBuildingShadowio = std::make_shared<BuildingShadowIO>();
    }
}


void Engine::input(std::string path, std::string V){

    if (V == "eRaytracing")
    {
        m_mode = Mode::eRaytracing;
    }
    else if (V == "eVoxelEB")
    {
        m_mode = Mode::eVoxelEB;
    }
    else if(V == "eVoxelRT"){
        m_mode = Mode::eVoxelRT;
    }
    else if (V=="eBuildingShadow")
    {
        m_mode = Mode::eBuildingShadow;
    }



    if(m_mode == Mode::eRaytracing) {
        m_pFileio = std::make_shared<FileIO>();
        m_pFileio->readXml(path, m_mode);
        init(m_mode);
        m_pRaytracing->setup(appSetting, m_pRaytracingio);
        m_pRaytracing->upload(m_pFileio, m_pRaytracingio);

        m_pRaytracingio->definedDir = m_pFileio->m_pRaytracingXml->definedDir;
        m_pRaytracingio->projectDir = m_pFileio->m_pRaytracingXml->projectDir;
    }else if(m_mode == Mode::eVoxelEB){
        m_pFileio = std::make_shared<FileIO>();
        m_pFileio->readXml(path, m_mode);
        init(m_mode);

        m_pVoxeleb->setup(appSetting, m_pVoxelebio);
        m_pVoxeleb->upload(m_pFileio, m_pVoxelebio);

        m_pVoxelebio->definedDir = m_pFileio->m_pVoxelebXml->definedDir;
        m_pVoxelebio->projectDir = m_pFileio->m_pVoxelebXml->projectDir;
    }else if(m_mode == Mode::eVoxelRT){
        m_pFileio = std::make_shared<FileIO>();
        m_pFileio->readXml(path, m_mode);
        init(m_mode);

        m_pVoxelrt->setup(appSetting, m_pVoxelrtio);
        m_pVoxelrt->upload(m_pFileio, m_pVoxelrtio);

        m_pVoxelrtio->definedDir = m_pFileio->m_pVoxelrtXml->definedDir;
        m_pVoxelrtio->projectDir = m_pFileio->m_pVoxelrtXml->projectDir;
    }else if(m_mode == Mode::eBuildingShadow)
    {
        m_pFileio = std::make_shared<FileIO>();
        m_pFileio->readXml(path, m_mode);
        init(m_mode);

        m_pBuildingShadow->setup(appSetting, m_pBuildingShadowio);
        m_pBuildingShadow->upload(m_pFileio, m_pBuildingShadowio);

        m_pBuildingShadowio->defineDir = m_pFileio->m_pBuildingShadowXml->defineDir;
        m_pBuildingShadowio->projectDir = m_pFileio->m_pBuildingShadowXml->projectDir;
    }
    
}

void Engine::create() {

    if(m_mode == Mode::eRaytracing) {
        m_pRaytracing->create(m_pRaytracingio);
    }else if(m_mode == Mode::eVoxelEB){
        m_pVoxeleb->create(m_pVoxelebio);
    }else if(m_mode == Mode::eVoxelRT){
        m_pVoxelrt->create(m_pVoxelrtio);
    }else if (m_mode == Mode::eBuildingShadow)
    {
        m_pBuildingShadow->create(m_pBuildingShadowio);
    }
}

void Engine::run() {

    if(m_mode == Mode::eRaytracing)
        m_pRaytracing->run(m_pRaytracingio,m_pFileio);
    else if(m_mode == Mode::eVoxelEB)
        m_pVoxeleb->run(m_pVoxelebio,m_pFileio);
    else if(m_mode == Mode::eVoxelRT)
        m_pVoxelrt->run(m_pVoxelrtio,m_pFileio);
    else if (m_mode == Mode::eBuildingShadow)
        m_pBuildingShadow->run(m_pBuildingShadowio, m_pFileio);

}


void Engine::destroy() {

    if(m_mode == Mode::eRaytracing)
        m_pRaytracing->destroy(m_pRaytracingio);
    else if(m_mode == Mode::eVoxelEB)
        m_pVoxeleb->destroy(m_pVoxelebio);
    else if(m_mode == Mode::eVoxelRT)
        m_pVoxelrt->destroy(m_pVoxelrtio);
    else if (m_mode == Mode::eBuildingShadow)
        m_pBuildingShadow->destroy(m_pBuildingShadowio);
    appSetting.destroy();


}