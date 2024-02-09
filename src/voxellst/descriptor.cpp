//
// Created by admin on 2024/1/25.
//

#include "descriptor.h"


bool Descriptor::createDescriptor(std::shared_ptr<VoxellstIO> &modelio){

    auto & m_device = modelio->m_device;
    auto & bindings = modelio->m_bindings;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & m_descSetPool = modelio->m_descPool;
    auto & m_descSet = modelio->m_descSet;
    auto & voxelio = modelio->m_voxelio;
    auto & defined = modelio->m_defined;
    auto & meshio = modelio->m_meshio;
    auto & instanceio = modelio->m_instanceio;
    auto & virtualio = modelio->m_virtualio;
 //   auto & sceneio = modelio->m_sceneio;


    VkShaderStageFlags flags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
    bindings.addBinding(VoxellstbindingInd::spectral, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::fixedSpectral, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::thermal, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::tempe, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::canopy, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::meshLink, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::instanceLink, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::voxelLink, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1,flags);
    bindings.addBinding(VoxellstbindingInd::nano, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::tlas, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1,flags | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    bindings.addBinding(VoxellstbindingInd::sensor, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::wave, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::light, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::atomcond, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::dir, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::rads, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::netRad, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::pnet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);

    bindings.addBinding(VoxellstbindingInd::storage, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags );
    bindings.addBinding(VoxellstbindingInd::meteo, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::aerocond, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);

    bindings.addBinding(VoxellstbindingInd::raa, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::leafBio,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::soilSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);

    bindings.addBinding(VoxellstbindingInd::rss, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::air, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(VoxellstbindingInd::flux, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);

    bindings.addBinding(VoxellstbindingInd::tLast, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);

    bindings.addBinding(VoxellstbindingInd::state, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);



    m_descSetLayout = bindings.createLayout(m_device);
    m_descSetPool = bindings.createPool(m_device, 1);
    m_descSet = nvvk::allocateDescriptorSet(m_device, m_descSetPool, m_descSetLayout);

    std::vector<VkWriteDescriptorSet> updates;

    // component    

    VkDescriptorBufferInfo dbiSpectral{meshio->m_pBufferSpectral->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::spectral, &dbiSpectral));


    VkDescriptorBufferInfo dbiFixedSpectral{meshio->m_pFixedSpectralBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::fixedSpectral, &dbiFixedSpectral));

    if (meshio->thermals.size() > 0)
    {
        VkDescriptorBufferInfo dbiThermal{meshio->m_pBufferThermal->buffer, 0, VK_WHOLE_SIZE};
        updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::thermal, &dbiThermal));
    }

    VkDescriptorBufferInfo dbiTempe{voxelio->m_pTempeBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::tempe, &dbiTempe));

    VkDescriptorBufferInfo dbiCanopy{meshio->m_pBufferCanopy->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::canopy, &dbiCanopy));

    // link

    VkDescriptorBufferInfo dbiMeshLink{meshio->m_pBufferMeshLink->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::meshLink, &dbiMeshLink));

    VkDescriptorBufferInfo dbiInstanceLink{instanceio->m_pBufferInstanceLink->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::instanceLink, &dbiInstanceLink));

    VkDescriptorBufferInfo dbiVoxelLink{voxelio->m_pVoxelLinkBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::voxelLink, &dbiVoxelLink));

    VkDescriptorBufferInfo dbiNano{voxelio->m_pVoxelNanoBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::nano, &dbiNano));

    // Accelerate structure
    VkAccelerationStructureKHR tlas = modelio->m_pAccelStruct->getTlas();
    VkWriteDescriptorSetAccelerationStructureKHR descASInfo{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR};
    descASInfo.accelerationStructureCount = 1;
    descASInfo.pAccelerationStructures = &tlas;
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::tlas, &descASInfo));

    // sensor and light
    VkDescriptorBufferInfo dbiSensor{modelio->m_pBufferSensor->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::sensor, &dbiSensor));
    VkDescriptorBufferInfo dbiWave{modelio->m_pBufferWave->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::wave, &dbiWave));
    VkDescriptorBufferInfo dbiLight{modelio->m_pBufferLight->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::light, &dbiLight));


    VkDescriptorBufferInfo dbiAtomcond{modelio->m_pBufferAtomcond->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::atomcond, &dbiAtomcond));

    VkDescriptorBufferInfo dbiDir{voxelio->m_pDirBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::dir, &dbiDir));
    VkDescriptorBufferInfo dbiRads{voxelio->m_pRadsBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::rads, &dbiRads));
    VkDescriptorBufferInfo dbiNetRad{voxelio->m_pNetRadBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::netRad, &dbiNetRad));
    VkDescriptorBufferInfo dbiPnet{voxelio->m_pPnetBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::pnet, &dbiPnet));

    VkDescriptorBufferInfo dbiStore{virtualio->m_pBufferStorage->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::storage, &dbiStore));

    VkDescriptorBufferInfo dbiMeteo{modelio->m_pMeteoBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::meteo, &dbiMeteo));

    VkDescriptorBufferInfo dbiAero{modelio->m_pBufferAero->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::aerocond, &dbiAero));


    VkDescriptorBufferInfo dbiRaa{voxelio->m_pRaaBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::raa, &dbiRaa));


    VkDescriptorBufferInfo dbiLeafbio{meshio->m_pLeafBioBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::leafBio, &dbiLeafbio));


    VkDescriptorBufferInfo dbiSoilset{meshio->m_pSoilSetBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::soilSet, &dbiSoilset));

    VkDescriptorBufferInfo dbiRss{voxelio->m_pRssBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::rss, &dbiRss));

    VkDescriptorBufferInfo dbiAir{voxelio->m_pAirBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::air, &dbiAir));

    VkDescriptorBufferInfo dbiFlux{voxelio->m_pFluxBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::flux, &dbiFlux));

    VkDescriptorBufferInfo dbiTlast{voxelio->m_pTLASTBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::tLast, &dbiTlast));

    VkDescriptorBufferInfo dbiState{voxelio->m_pStateBuffer->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, VoxellstbindingInd::state, &dbiState));

    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(updates.size()), updates.data(), 0, nullptr);
    return true;
}

void Descriptor::destroy(std::shared_ptr<VoxellstIO> &modelio) {

    vkDestroyDescriptorPool(modelio->m_device, modelio->m_descPool, nullptr);
    vkDestroyDescriptorSetLayout(modelio->m_device, modelio->m_descSetLayout, nullptr);


    modelio->m_descPool = VkDescriptorPool();
    modelio->m_descSetLayout = VkDescriptorSetLayout();
    modelio->m_descSet = VkDescriptorSet();


}