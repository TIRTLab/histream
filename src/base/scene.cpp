//
// Created by admin on 2024/1/24.
//

#include "scene.h"


bool Scene::createObjScene(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &raytracingio) {

    bool isInterp = false;
    auto & meshio = raytracingio->m_meshio;
    auto & instanceio = raytracingio->m_instanceio;
    auto & scenexml = fileio->m_pRaytracingXml->scenexml;

    //auto & sceneio = raytracingio->m_sceneio;

    ObjLoader loader;

    //-------------------------
    //-- Background Mesh
    //-------------------------
    if(scenexml.isDEM == true) {
        return false;
    }else{
        loader.createBackground(fileio->m_pRaytracingXml->scenexml.sceneSize);
    }
    //n_modelmesh = 0;
    int & n_modelmesh = raytracingio->n_modelmesh;
    loader.m_objmesh.meshId = n_modelmesh;
    meshio->objMeshes.emplace_back(loader.m_objmesh);
    //-------------------------
    //-- Background MeshLink
    //-------------------------
    MeshLink bgMeshLink{};
    bgMeshLink.type = int(Type::SOIL);
    std::string bgSpectralName = fileio->m_pRaytracingXml->scenexml.background.spectralName;
    bgMeshLink.spectralId =  meshio->spectralNames.find(bgSpectralName)->second;
    std::string bgThermalName;
    int bgThermalIndex = 0;
    if (fileio->m_pRaytracingXml->sensorxml.isTemperature == true)
    {
        bgThermalName =  fileio->m_pRaytracingXml->scenexml.background.thermalName;
        bgThermalIndex = meshio->thermalNames.find(bgThermalName)->second;
    }
    bgMeshLink.thermalId = bgThermalIndex;
    meshio->meshLinks.emplace_back(bgMeshLink);
    //-------------------------
    //-- Background Instance
    //-------------------------
    Instance bgInstance{};
    bgInstance.meshId = static_cast<uint32_t>(n_modelmesh);
    n_modelmesh++;
    glm::mat4 bgunit = glm::mat4(1.0f);
    glm::vec3 bgShift = glm::vec3{0, 0 - loader.minElevation, 0};
    glm::vec3 bgScale = glm::vec3{1.0, 1.0, 1.0};
    glm::mat4 bgMat = glm::scale(bgunit, bgScale) * glm::translate(bgunit,bgShift);
    bgInstance.object2worldMatrix = bgMat;
    bgInstance.world2objectMatrix = glm::transpose(glm::inverse(bgMat));
    instanceio->instances.emplace_back(bgInstance);


    //-------------------------
    //-- Background InstanceLink
    //-------------------------
    InstanceLink bgInstanceLink{};
    bgInstanceLink.meshId = bgInstance.meshId;
    instanceio->instanceLinks.emplace_back(bgInstanceLink);

    /// ------------------------------------
    /// Scene
    ///-------------------------------------
    float x = fileio->m_pRaytracingXml->scenexml.sceneSize.x; // lenght
    float y = fileio->m_pRaytracingXml->scenexml.sceneSize.y; // width
    float z = fileio->m_pRaytracingXml->scenexml.sceneSize.z; // height
    raytracingio->sMin = glm::vec3(-x / 2.0, 0, -y / 2.0);
    raytracingio->sMax = glm::vec3(x / 2.0, z, y / 2.0);
    raytracingio->sceneSize = glm::vec3(x,y,z);
    raytracingio->sceneOrigin = fileio->m_pRaytracingXml->scenexml.sceneOrigin;


   // outputModel(loader.m_objmesh, m_pRaytracingXml->setting.outDir + "/background.obj");
   // std::cout << "Export background.obj" << std::endl;



    //-------------------------
    //-- Obj Models
    //-------------------------
    int n_obj = fileio->m_pRaytracingXml->scenexml.objEntities.size();
    for (int kobj = 0; kobj < n_obj; kobj++)
    {
        auto &objEntity = fileio->m_pRaytracingXml->scenexml.objEntities[kobj];
        std::string fileName = objEntity.filePath;
        std::string objName = objEntity.objName;
        int n_mesh = objEntity.meshNames.size();
        for (int kmesh = 0; kmesh < n_mesh; kmesh++)
        {
            /// ------------------------------------
            /// model/mesh
            ///-------------------------------------
            std::string meshName = objEntity.meshNames[kmesh];
            std::string spectralName = objEntity.spectralNames[kmesh];
            std::string thermalName;

            loader.loadMesh(fileName, meshName);
            loader.m_objmesh.meshId = n_modelmesh;
            meshio->objMeshes.emplace_back(loader.m_objmesh);

            /// ------------------------------------
            /// Mesh Link
            ///-------------------------------------

            int spectralIndex = meshio->spectralNames.find(spectralName)->second;
            int thermalIndex = 0;
            if (fileio->m_pRaytracingXml->sensorxml.isTemperature == true)
            {
                thermalName = objEntity.thermalNames[kmesh];
                thermalIndex = meshio->thermalNames.find(thermalName)->second;
            }
            MeshLink meshLink{};
            meshLink.type = int(Type::VEGETATION);
            meshLink.spectralId = spectralIndex;
            meshLink.thermalId = thermalIndex;
            meshio->meshLinks.emplace_back(meshLink);

            int n_instancet = objEntity.objDistributions.size();
            for (int kinstance = 0; kinstance < n_instancet; kinstance++)
            {
                /// ------------------------------------
                /// Instance
                ///-------------------------------------
                Instance instance{};
                instance.meshId = static_cast<uint32_t>(n_modelmesh);
                glm::vec3 shift0 = objEntity.objDistributions[kinstance];
                float scale0 = objEntity.scales[kinstance];
                float angle0 = objEntity.rotations[kinstance];

                //nvmath::vec3f shift = nvmath::vec3f{ shift0.x - x/2.0,shift0.z,shift0.y - z/2.0 };
                glm::vec3 shift;
                if (isInterp == true)
                {
                    // double shiftInterp = loader.getShiftInterp(interp, shift0);
                    // shift = nvmath::vec3f{shift0.x - x / 2.0, shift0.z + shiftInterp - loader.minElevation, shift0.y - y / 2.0};
                }
                else
                {
                    shift = glm::vec3{shift0.x - x / 2.0, shift0.z, shift0.y - y / 2.0};
                }


                glm::mat4 unit = glm::mat4(1.0f);
                glm::vec3 scale = glm::vec3(scale0);
                glm::mat4 angle = glm::rotate(unit, glm::radians(angle0), glm::vec3(0.0,1.0,0.0));
                glm::mat4 mat = glm::scale(unit,scale) * glm::translate(unit, shift) * angle;
                instance.object2worldMatrix = mat;
                instance.world2objectMatrix = glm::transpose(glm::inverse(mat));
                instanceio->instances.emplace_back(instance);

                /// ------------------------------------
                /// InstanceLink
                ///-------------------------------------
                InstanceLink instanceLink{};
                instanceLink.meshId = instance.meshId;
                instanceio->instanceLinks.emplace_back(instanceLink);
            }
            n_modelmesh++;
        }
    }



    return true;
}

//-----------------------------------------------------------
//--- n_moxelmesh,n_instance,n_voxel,n_surf,n_grid
//---
//---
//------------------------------------------------------------


bool Scene::createPrimScene(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &voxellstio) {


    auto &scenexml = fileio->m_pVoxelLstXml->scenexml;
    auto &voxellstxml = fileio->m_pVoxelLstXml;
    auto &meshio = voxellstio->m_meshio;
    auto &instanceio = voxellstio->m_instanceio;
    auto &voxelio = voxellstio->m_voxelio;

    nanovdb::GridBuilder<int32_t > nanoBuilder(-1);
    auto acc = nanoBuilder.getAccessor();

    /// ------------------------------------
    /// Scene
    ///-------------------------------------
    float stepSize = fileio->m_pVoxelLstXml->scenexml.stepsize_surface;
    float x = fileio->m_pVoxelLstXml->scenexml.sceneSize.x; // lenght
    float y = fileio->m_pVoxelLstXml->scenexml.sceneSize.y; // width
    float z = fileio->m_pVoxelLstXml->scenexml.sceneSize.z; // height
    voxellstio->sMin = glm::vec3(-x / 2.0, 0, -y / 2.0);
    voxellstio->sMax = glm::vec3(x / 2.0, z, y / 2.0);
    voxellstio->sceneSize = glm::vec3(x, y, z);
    voxellstio->sceneOrigin = fileio->m_pVoxelLstXml->scenexml.sceneOrigin;
    voxellstio->voxelSize = glm::ivec3(x/stepSize,0,y/stepSize);
    //voxellstio->n_surface = voxellstio->voxelSize.x * voxellstio->voxelSize.y;

    int &n_modelmesh = voxellstio->n_modelmesh;
    int &n_instance = voxellstio->n_instance;
    int &n_voxel = voxellstio->n_voxel;

    /// ------------------------------------
    /// BACKGROUND mesh
    ///-------------------------------------

    VoxelDesigner loader;
//    VoxelTriModel bgModelXYZ;
    PrimMesh bgModelXYZ;
    if ((scenexml.isDEM == true) && (scenexml.DEMPath != ""))
    {

    }
    else
    {
        bgModelXYZ = loader.createTriBackground(scenexml.sceneSize.x, scenexml.sceneSize.y, scenexml.stepsize_surface);
        //bgModelXYZ = loader.createTriBackground(sceneSizeX, scensSizeY, 5);
    }


    bgModelXYZ.meshId = n_modelmesh;
    PrimMesh bgModel = XYZ2XZY(bgModelXYZ);
    //voxellistio->m_meshio->objmeshes.emplace_back(bgModel);
    meshio->primMeshes.emplace_back(bgModel);

    // background model link
    MeshLink bgMeshLink;
    std::string bgSpectralName = scenexml.background.spectralName;
    bgMeshLink.spectralId =  meshio->spectralNames.find(bgSpectralName)->second;
    //int bgSpectralId = m_pXmlInput->compo.spectralNames.find(bgSpectralName)->second;
   // int bgSpectralId = getSpectralID(bgSpectralName);
//    std::string bgThermalName;
//    int bgThermalIndex = 0;
//    if (fileio->m_pVoxelLstXml->sensorxml.isTemperature == true)
//    {
//        bgThermalName = scenexml.background.thermalName;
//        bgThermalIndex = meshio->thermalNames.find(bgThermalName)->second;
//    }

    int bgCanopyindex = 0;
  //  std::string bgCanopyName = scenexml.background.canopyName;
  //  bgMeshLink.canopyId = meshio->canopyNames.find(bgCanopyName)->second;
    int bgPropIndex = 0;
    std::string bgPropName = scenexml.background.bgPropName;
    bgPropIndex = meshio->soilsetNames.find(bgPropName)->second;
    bgMeshLink.bioId = bgPropIndex;
    bgMeshLink.type = (int)Type::SOIL;
    meshio->meshLinks.emplace_back(bgMeshLink);

    /// ------------------------------------
    /// BACKGROUND instance
    ///-------------------------------------
    Instance bgInstance{};
    bgInstance.meshId = static_cast<uint32_t>(n_modelmesh);
    glm::mat4 bgunit = glm::mat4(1.0f);
//    nvmath::vec3f{-sceneSizeX / 2.0, -1, -scensSizeY / 2.0};
    glm::vec3 bgShift = glm::vec3{-voxellstio->voxelSize.x/2.0, 0 - loader.minElevation, -voxellstio->voxelSize.z/2.0};
    glm::vec3 bgScale = glm::vec3{1.0, 1.0, 1.0};
    glm::mat4 bgMat = glm::scale(bgunit, bgScale) * glm::translate(bgunit,bgShift);
    bgInstance.object2worldMatrix = bgMat;
    bgInstance.world2objectMatrix = glm::transpose(glm::inverse(bgMat));
    instanceio->instances.emplace_back(bgInstance);


    // background instanceLink
    InstanceLink bgInstanceLink{};
    bgInstanceLink.meshId = bgInstance.meshId;
    instanceio->instanceLinks.emplace_back(bgInstanceLink);

    //--------------------------------------------
    // background voxelLink
    //--------------------------------------------
    for (int kvoxel = 0; kvoxel < bgModel.centers.size(); kvoxel++)
    {
        glm::ivec3 pos = glm::ivec3(bgModel.centers[kvoxel]) ;// + nvmath::vec3i(bgShift);
       // glm::ivec3 newpos = bgMat * glm::vec4(pos,1.0);
        int test = acc.getValue(nanovdb::Coord(pos.x, pos.y, pos.z)); //(1,0,1)

        if (test < 0)
        {
            acc.setValue(nanovdb::Coord(pos.x, pos.y, pos.z), n_voxel);
            VoxelLink voxellink{};
            glm::ivec3 voxelPos = glm::ivec3(pos.x * 1.0, pos.y * 1.0, pos.z * 1.0);
            voxellink.voxelPos = voxelPos;
            voxellink.instanceId = n_instance;
            voxellink.primId = 1;
            if(fileio->m_pVoxelLstXml->aerocondxml.aerotype==AeroType::one){
                voxellink.aeroId = 0;
            }

//            voxellink.meshId = n_modelmesh;
            voxelio->voxellinks.emplace_back(voxellink);
            n_voxel++;
        }
    }
   // n_voxel++;
    n_instance++;
    n_modelmesh++;
    //voxelIdOffset += bgModel.centerPoints.size();


    /// ------------------------------------
    /// voxel Components
    ///-------------------------------------
//    for (int kVoxelModel = 0; kVoxelModel < scenexml.primEntities.size(); kVoxelModel++)
//    {
//        auto &voxelEntity = scenexml.primEntities[kVoxelModel];
//
//        bool isInterp = false;
//        /// ------------------------------------
//        /// voxel model/mesh
//        ///-------------------------------------
//        std::string modelName = voxelEntity.primitiveName;
//        std::string meshName = voxelEntity.meshNames[0];
//        std::string spectralName = voxelEntity.spectralNames[0];
//        std::string thermalName;
////        if (voxellstxml->sensorxml.isTemperature == true)
////        {
////            thermalName = voxelEntity.thermalNames[0];
////        }
//        std::string canopyName = voxelEntity.canopyNames[0];
//        std::string propName = voxelEntity.propNames[0];
//        Type type = voxelEntity.types[0];
//        //meshio->types[0] = voxelEntity.types[0];
//       // std::string aeroName = voxelEntity.aeroNames[0];
//
//        PrimMesh currentVoxelModelXYZ;
//
//        if (meshName == "Rotate")
//        {
//            currentVoxelModelXYZ = loader.createTriEntity(voxelEntity.shapetypes[0], voxelEntity.shapes[0], scenexml.stepsize_surface);
//        }
//        else
//        {
//            /*currentVoxelModelXYZ = loader.createTriEntity(voxelEntity.shapeTypes[0], voxelEntity.shapes[0], stepSize);*/
//            currentVoxelModelXYZ = loader.createTriEntity(voxelEntity.shapetypes[0], voxelEntity.shapes[0], scenexml.stepsize_surface);
//        }
//        PrimMesh currentVoxelModel = XYZ2XZY(currentVoxelModelXYZ); // (1,1,0) => (1,0,1) with  height = 0
//        currentVoxelModel.meshId = n_modelmesh;
//        meshio->primMeshes.emplace_back(currentVoxelModel); //xzy
//
//        //outputModel(currentVoxelModel, m_pXmlInput->setting.outDir + "/voxelEntity.obj");
//        /* if (meshName == "Rotate")
//         {
//             outputModel(currentVoxelModel, m_pXmlInput->setting.outDir + "/voxelEntity.obj");
//         }*/
//        // voxel attribute
//        //int spectralIndex = m_pXmlInput->compo.spectralNames.find(spectralName)->second;
//        MeshLink meshlink;
//        meshlink.spectralId = meshio->spectralNames.find(spectralName)->second;
//        int thermalIndex = 0;
////        if (voxellstxml->sensorxml.isTemperature == true)
////        {
////            meshlink.thermalId = meshio->thermalNames.find(thermalName)->second;
////        }
//        meshlink.canopyId = meshio->canopyNames.find(canopyName)->second;
//        if(type == Type::VEGETATION) {
//            // meshlink.leafbioId = meshio->leafbioNames.find(propName)->second;
//            meshlink.bioId = meshio->leafbioNames.find(propName)->second;
//        }else if(type ==Type::SOIL){
//          //  meshlink.soilsetId = meshio->soilsetNames.find(propName)->second;
//            meshlink.bioId = meshio->soilsetNames.find(propName)->second;
//        }
//      //  meshlink.aeroId = meshio->aeroNames.find(aeroName)->second;
//        meshlink.type = (int)type;
//        meshio->meshLinks.emplace_back(meshlink);
//
//
//        // instance
//
//        for (int kinstance = 0; kinstance < voxelEntity.primDistributions.size(); kinstance++)
//        {
//            // voxel instance��change postion,
//            Instance instance{};
//            instance.meshId = static_cast<uint32_t>(n_modelmesh);
//            glm::ivec3 shift0;
//            if (isInterp)
//            {
////                shift0 = {voxelEntity.primDistributions[kinstance].x,
////                          voxelEntity.primDistributions[kinstance].z + interp(voxelEntity.primDistributions[kinstance].x,
////                                                                          voxelEntity.primDistributions[kinstance].y),
////                          voxelEntity.primDistributions[kinstance].y};
//            }
//            else
//            {
//                shift0 = {voxelEntity.primDistributions[kinstance].x/scenexml.stepsize_surface,
//                          voxelEntity.primDistributions[kinstance].z/scenexml.stepsize_surface,
//                          voxelEntity.primDistributions[kinstance].y/scenexml.stepsize_surface};
//            }
//
//            // first, the scene coordinate become the voxel coordinate;
//            // then, link the coordinate between voxel and image, no matter on scene...
//
//            glm::ivec3 semiRange = glm::ivec3(
//                    voxellstio->sceneSize.x / scenexml.stepsize_surface / 2.0,
//                    0 ,
//                    voxellstio->sceneSize.y / scenexml.stepsize_surface / 2.0);
//
//            float scale0 = voxelEntity.scales[kinstance];
//            float angle0 = voxelEntity.rotations[kinstance];
//            glm::vec3 shift = glm::vec3(shift0)-glm::vec3(semiRange);  // (5,0,5)
//
//            //
//            glm::mat4 unit = glm::mat4(1.0f);
//            glm::vec3 scale = glm::vec3(scale0);
//            glm::mat4 angle = glm::rotate(unit, glm::radians(angle0), glm::vec3(0.0,1.0,0.0));
//            glm::mat4 mat = glm::scale(unit,scale) * glm::translate(unit, shift);
//
//            instance.object2worldMatrix = mat;
//            instance.world2objectMatrix = glm::transpose(glm::inverse(mat));
//            instanceio->instances.emplace_back(instance);
//
//            // voxel Instance Link
//            InstanceLink instancelink{};
//            instancelink.meshId = instance.meshId;
//            instanceio->instanceLinks.emplace_back(instancelink);
//
//
//            // voxel coordinate
//            int isValid = 0;
//            for (int kvoxel = 0; kvoxel < currentVoxelModel.centers.size(); kvoxel++)
//            {
//                // this is what we did in the shader;
//                glm::ivec3 pos =  mat * glm::vec4(currentVoxelModel.centers[kvoxel],1.0);
//                int test = acc.getValue(nanovdb::Coord(pos.x, pos.y, pos.z));
//
//
//                int ind = (pos.z) * voxellstio->voxelSize.x + (pos.x);
//                //int ind = (scensSizeY - pos.z) * sceneSizeX + (sceneSizeX - pos.x);
//                //ind = sceneSizeX * scensSizeY - ind;
//                //int ind = (pos.x + sceneSizeX / 2) * scensSizeY + (pos.z + scensSizeY / 2);
//                // int ind = (sceneSizeX - pos.x) * scensSizeY + (scensSizeY - pos.z);
//
//
////                if ((voxelio->surfLs.size() > ind) && (voxelio->surfLs[ind].hvmax < (pos.y * stepSize)))
////                    m_pVLstInput->surfLs[ind].hvmax = pos.y *stepSize;
//
//                if (test < 0)
//                {
//                    acc.setValue(nanovdb::Coord(pos.x, pos.y, pos.z), n_voxel);
//                    VoxelLink voxelLink{};
//                    glm::ivec3 voxelId = glm::ivec3(pos.x * 1.0, pos.y * 1.0, pos.z * 1.0);  //(5,0,5) with height = 0
//                    voxelLink.voxelPos = voxelId;          //(5,0,5) with height = 0
//                    voxelLink.instanceId = n_instance;
//
//                    //// acumulating lai
////                    if (m_pVLstInput->surfLs.size() > ind && instanceLink.type == (int)Type::VEGETATION)
////                    {
////                        float density = m_pVLstInput->canopies[canopyIndex].density;
////                        float lai = density * stepSize;
////                        m_pVLstInput->surfLs[ind].laimax += lai;
////                    }
//
//                    // ������أ�ÿ�����ؾ�����vl
//                    voxellstio->m_voxelio->voxellinks.emplace_back(voxelLink);
//                    n_voxel++;
//                    isValid = 1;
//                }
//            }
//            if (isValid == 0)
//            {
//                continue;
//            }
//
//            n_instance++;
//        }
//
//
//        n_modelmesh++;
//        std::string info = "voxel entity " + std::to_string(kVoxelModel) + " done.\n";
//        //LOGI(info.c_str());
//    }

   // float tt = acc.getValue(nanovdb::Coord(24, 0, 24));
    voxellstio->m_voxelio->nanoHandle = nanoBuilder.getHandle<>();


    return true;
}

PrimMesh Scene::XYZ2XZY(PrimMesh model){
    PrimMesh temp;
    temp.meshId = model.meshId;
    temp.nIndices = model.nIndices;
    temp.nVertices = model.nVertices;
    temp.indices = model.indices;
    for (int i = 0; i< model.nVertices;i++)
    {
        VertexAttribute vertex = {};
        vertex.color = model.vertices[i].color;
        vertex.nrm = model.vertices[i].nrm;
        vertex.texCoord = model.vertices[i].texCoord;
        vertex.pos = {model.vertices[i].pos.x,
                      model.vertices[i].pos.z,
                      model.vertices[i].pos.y};
        temp.vertices.emplace_back(vertex);
    }
    for (int i = 0; i < model.centers.size(); i++)
    {
        glm::vec3 center = {
                model.centers[i].x,
                model.centers[i].z,
                model.centers[i].y};
        temp.centers.emplace_back(center);
    }

    return temp;
}

ObjMesh Scene::XYZ2XZY(ObjMesh model){
    ObjMesh temp;
    temp.meshId = model.meshId;
    temp.nIndices = model.nIndices;
    temp.nVertices = model.nVertices;
    temp.indices = model.indices;
    for (int i = 0; i< model.nVertices;i++)
    {
        VertexAttribute vertex = {};
        vertex.color = model.vertices[i].color;
        vertex.nrm = model.vertices[i].nrm;
        vertex.texCoord = model.vertices[i].texCoord;
        vertex.pos = {model.vertices[i].pos.x,
                      model.vertices[i].pos.z,
                      model.vertices[i].pos.y};
        temp.vertices.emplace_back(vertex);
    }


    return temp;
}