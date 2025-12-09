//
// Created by Fan Tengyuan on 9/12/2025.
//
#include "descriptor.h"

bool Descriptor::create(std::shared_ptr<VoxelizationIO> &modelio) {
    auto & m_device = modelio->m_device;
    auto & bindings = modelio->m_bindings;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & m_descPool = modelio->m_descPool;
    auto & m_descSet = modelio->m_descSet;
    auto & meshio = modelio->m_meshio;
    auto & instanceio = modelio->m_instanceio;
    auto & virtualio = modelio->m_virtualio;

    bindings.addBinding(BindingInd::spectral, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    if(meshio->thermals.size() > 0)
    {
        bindings.addBinding(BindingInd::thermal, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    }
    bindings.addBinding(BindingInd::modelLink, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    bindings.addBinding(BindingInd::instanceLink, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    bindings.addBinding(BindingInd::tlas, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    bindings.addBinding(BindingInd::sensor, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    bindings.addBinding(BindingInd::light, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    bindings.addBinding(BindingInd::wave, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    bindings.addBinding(BindingInd::storage, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);

    m_descSetLayout = bindings.createLayout(m_device);
    m_descPool = bindings.createPool(m_device, 1);
    m_descSet = nvvk::allocateDescriptorSet(m_device, m_descPool, m_descSetLayout);

    /// <summary>
    /// get needed buffers and update the descriptor sets.
    /// temBuffer needed corrected.
    /// </summary>
    std::vector<VkWriteDescriptorSet> updates;
    VkDescriptorBufferInfo dbiSpectral{ meshio->m_pBufferSpectral->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, BindingInd::spectral, &dbiSpectral));
    if (meshio->thermals.size() > 0)
    {
        VkDescriptorBufferInfo dbiThermal{ meshio->m_pBufferThermal->buffer, 0, VK_WHOLE_SIZE };
        updates.emplace_back(bindings.makeWrite(m_descSet, BindingInd::thermal, &dbiThermal));
    }
    VkDescriptorBufferInfo dbiModelLink{meshio->m_pBufferMeshLink->buffer, 0, VK_WHOLE_SIZE };
    updates.emplace_back(bindings.makeWrite(m_descSet, BindingInd::modelLink, &dbiModelLink));

    VkDescriptorBufferInfo dbiInstance{ instanceio->m_pBufferInstanceLink->buffer, 0, VK_WHOLE_SIZE };
    updates.emplace_back(bindings.makeWrite(m_descSet, BindingInd::instanceLink, &dbiInstance));

    VkAccelerationStructureKHR tlas = modelio->m_pAccelStruct->getTlas(); // get from m_accel
    VkWriteDescriptorSetAccelerationStructureKHR descASInfo{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR };
    descASInfo.accelerationStructureCount = 1;
    descASInfo.pAccelerationStructures = &tlas;
    updates.emplace_back(bindings.makeWrite(m_descSet, BindingInd::tlas, &descASInfo));

    VkDescriptorBufferInfo dbiSensor{ modelio->m_pBufferSensor->buffer, 0, VK_WHOLE_SIZE };
    updates.emplace_back(bindings.makeWrite(m_descSet, BindingInd::sensor, &dbiSensor));
    VkDescriptorBufferInfo dbiLight{ modelio->m_pBufferLight->buffer, 0, VK_WHOLE_SIZE };
    updates.emplace_back(bindings.makeWrite(m_descSet, BindingInd::light, &dbiLight));
    VkDescriptorBufferInfo dbiWave{ modelio->m_pBufferWave->buffer, 0, VK_WHOLE_SIZE };
    updates.emplace_back(bindings.makeWrite(m_descSet, BindingInd::wave, &dbiWave));

    VkDescriptorBufferInfo dbiStorage{ virtualio->m_pBufferStorage->buffer, 0, VK_WHOLE_SIZE };
    updates.emplace_back(bindings.makeWrite(m_descSet, BindingInd::storage, &dbiStorage));

    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(updates.size()), updates.data(), 0, nullptr);
    return false;
}

