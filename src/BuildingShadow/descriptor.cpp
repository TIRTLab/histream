//
// Created by ShIP on 2024/12/2.
//
#include "descriptor.h"

bool Descriptor::createDescriptor(std::shared_ptr<BuildingShadowIO>& modelio)
{
    auto & m_device = modelio->m_device;
    auto & bindings = modelio->m_bindings;
    auto & m_descSetLayout = modelio->m_descSetLayout;
    auto & m_descPool = modelio->m_descPool;
    auto & m_descSet = modelio->m_descSet;
    auto & meshio = modelio->m_meshio;
    auto & instanceio = modelio->m_instanceio;
    // auto & virtualio = modelio->m_virtualio;
    //   auto & sceneio = modelio->m_sceneio;

    VkShaderStageFlags flags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_INTERSECTION_BIT_KHR;

    bindings.addBinding(ShadowBindingInd::facetInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT | VK_SHADER_STAGE_RAYGEN_BIT_KHR);
    bindings.addBinding(ShadowBindingInd::tlas, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1, flags | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR);
    bindings.addBinding(ShadowBindingInd::meshLink, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);
    bindings.addBinding(ShadowBindingInd::instanceLink, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, flags);

    m_descSetLayout = bindings.createLayout(m_device);
    m_descPool = bindings.createPool(m_device, 1);
    m_descSet = nvvk::allocateDescriptorSet(m_device, m_descPool, m_descSetLayout);

    /// <summary>
    /// get needed buffers and update the descriptor sets.
    /// temBuffer needed corrected.
    /// </summary>
    std::vector<VkWriteDescriptorSet> updates;

    VkDescriptorBufferInfo dbiClassId{modelio->m_pFacetInfoBuffer->buffer, 0, VK_WHOLE_SIZE };
    updates.emplace_back(bindings.makeWrite(m_descSet, ShadowBindingInd::facetInfo, &dbiClassId));

    VkAccelerationStructureKHR tlas = modelio->m_pAccelStruct->getTlas(); // get from m_accel
    VkWriteDescriptorSetAccelerationStructureKHR descASInfo{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR };
    descASInfo.accelerationStructureCount = 1;
    descASInfo.pAccelerationStructures = &tlas;
    updates.emplace_back(bindings.makeWrite(m_descSet, ShadowBindingInd::tlas, &descASInfo));


    VkDescriptorBufferInfo dbiMeshLink{meshio->m_pBufferMeshLink->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, ShadowBindingInd::meshLink, &dbiMeshLink));

    VkDescriptorBufferInfo dbiInstanceLink{instanceio->m_pBufferInstanceLink->buffer, 0, VK_WHOLE_SIZE};
    updates.emplace_back(bindings.makeWrite(m_descSet, ShadowBindingInd::instanceLink, &dbiInstanceLink));


    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(updates.size()), updates.data(), 0, nullptr);
    return true;
}

void Descriptor::destroy(std::shared_ptr<BuildingShadowIO>& modelio)
{
    vkDestroyDescriptorPool(modelio->m_device, modelio->m_descPool, nullptr);
    vkDestroyDescriptorSetLayout(modelio->m_device, modelio->m_descSetLayout, nullptr);


    modelio->m_descPool = VkDescriptorPool();
    modelio->m_descSetLayout = VkDescriptorSetLayout();
    modelio->m_descSet = VkDescriptorSet();
}
