//
// Created by ShIP on 2024/12/2.
//
#include "buffer.h"

bool Buffer::createBuffer(std::shared_ptr<BuildingShadowIO>& modelio)
{
    ///--------------------------------------------------------------------
    ///  component properties
    ///--------------------------------------------------------------------

    VkDevice & m_device = modelio->m_device;
    nvvk::Queue &m_queue  = modelio->m_queues[eGCT];
    auto & meshio = modelio->m_meshio;
    auto & instanceio = modelio->m_instanceio;
    // auto & virtualio = modelio->m_virtualio;
    auto & m_pAlloc = modelio->m_pAlloc;
    auto & m_pAccelStruct = modelio->m_pAccelStruct;
    auto n_facet = modelio->n_facet;

    nvvk::CommandPool cmdGen(m_device, m_queue.familyIndex);

    ///--------------------------------------------------------------------
    ///  Scene properties
    ///--------------------------------------------------------------------
    // models buffer
    nvvk::CommandPool cmdBufGet(m_device, m_queue.familyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_queue.queue);
    for (int kmodel = 0; kmodel < meshio->objMeshes.size(); kmodel++)
    {

        VkCommandBuffer cmdBufModel = cmdBufGet.createCommandBuffer();
        MeshBuffer meshbuffer;
        meshbuffer.nbVertices = static_cast<uint32_t>(meshio->objMeshes[kmodel].nVertices);
        meshbuffer.nbIndices = static_cast<uint32_t>(meshio->objMeshes[kmodel].nIndices);

        meshbuffer.indexBuffer =
                m_pAlloc->createBuffer(cmdBufModel, meshio->objMeshes[kmodel].indices,
                                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
                                       | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
        meshbuffer.vertexBuffer =
                m_pAlloc->createBuffer(cmdBufModel, meshio->objMeshes[kmodel].vertices,
                                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
                                       | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
        cmdBufGet.submitAndWait(cmdBufModel);
        //m_pAlloc->finalizeAndReleaseStaging();
        meshio->m_bufferMeshes.emplace_back(meshbuffer);
    }

    // modelLink
    VkCommandBuffer cmdBufModelLink = cmdGen.createCommandBuffer();
    for (int kmodel = 0; kmodel < meshio->objMeshes.size(); kmodel++)
    {

        meshio->meshLinks[kmodel].vertexAddress = nvvk::getBufferDeviceAddress(m_device, meshio->m_bufferMeshes[kmodel].vertexBuffer.buffer);
        meshio->meshLinks[kmodel].indexAddress = nvvk::getBufferDeviceAddress(m_device, meshio->m_bufferMeshes[kmodel].indexBuffer.buffer);
    }
    meshio->m_pBufferMeshLink = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufModelLink, meshio->meshLinks, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
    cmdGen.submitAndWait(cmdBufModelLink);

    // instanceLink : add address first and then creat
    VkCommandBuffer cmdBufLink = cmdGen.createCommandBuffer();
    if (instanceio->instanceLinks.size() > 0)
        instanceio->m_pBufferInstanceLink = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufLink, instanceio->instanceLinks, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
    cmdGen.submitAndWait(cmdBufLink);

    // tempe
    VkCommandBuffer cmdBufFacetInfo = cmdGen.createCommandBuffer();
    std::vector<FacetInfo> facetInfos(n_facet, FacetInfo{0, 0});
    modelio->m_pFacetInfoBuffer = std::make_shared<nvvk::Buffer>(m_pAlloc->createBuffer(cmdBufFacetInfo, facetInfos,
                                                                           VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
    cmdGen.submitAndWait(cmdBufFacetInfo);

    ///--------------------------------------------------------------------
    ///  Accelerate properties
    ///--------------------------------------------------------------------
    m_pAccelStruct->createAccelStruct(m_device,meshio,instanceio);

    m_pAlloc->finalizeAndReleaseStaging();
    return false;
}

void Buffer::destroy(std::shared_ptr<BuildingShadowIO>& modelio)
{
    VkDevice & m_device = modelio->m_device;
    nvvk::Queue &m_queue  = modelio->m_queues[eGCT];
    auto & meshio = modelio->m_meshio;
    auto & instanceio = modelio->m_instanceio;
    auto & m_pAlloc = modelio->m_pAlloc;


    // auto & virtualio = modelio->m_virtualio;
    // auto & voxelio = modelio->m_voxelio;
    // auto n_voxel = modelio->n_voxel;
    // auto & m_pAccelStruct = modelio->m_pAccelStruct;
    // auto & defined = modelio->m_defined;
    // auto & surfio = modelio->m_surfio;

    for (int i = 0; i < meshio->m_bufferMeshes.size(); i++)
    {
        m_pAlloc->destroy((meshio->m_bufferMeshes[i].vertexBuffer));
        m_pAlloc->destroy((meshio->m_bufferMeshes[i].indexBuffer));
    }

    m_pAlloc->destroy(*(modelio->m_pFacetInfoBuffer));
    m_pAlloc->destroy(*(modelio->m_meshio->m_pBufferMeshLink));
    m_pAlloc->destroy(*(modelio->m_instanceio->m_pBufferInstanceLink));
}
