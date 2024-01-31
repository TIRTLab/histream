//
// Created by admin on 2024/1/26.
//

#include "command.h"



bool Command::create(std::shared_ptr<VoxellstIO> &modelio){
    // for the firest init
    VkFenceCreateInfo fci = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    vkCreateFence(modelio->m_device, &fci, nullptr, &modelio->m_fence);

    // Here 4 means number of pipelines + 1
    modelio->m_semaphores.resize(2);
    std::generate_n( modelio->m_semaphores.begin(), 2,
                     [&]
                     {
                         VkSemaphoreCreateInfo sci = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
                         VkSemaphore semaphore;
                         vkCreateSemaphore( modelio->m_device, &sci, nullptr, &semaphore);
                         return semaphore;
                     });

    return false;
}

bool Command::run(std::shared_ptr<VoxellstIO> &modelio){

    modelio->m_currentSemaphore = 1;

    glm::ivec3 voxelSize1D = glm::ivec3((modelio->n_voxel + (GROUP_SIZEX - 1)) / GROUP_SIZEX, 1, 1);
    auto & descSet = modelio->m_descSet;
    auto & setting = modelio->setting;


    auto & piplineLayout = modelio->m_pipelineLayout_rt;
    auto & piplines = modelio->m_pipelines_rt;

    //--------------------------------------------------

    int stageInt = 0;


    submit(modelio, VoxellstStage::gap, voxelSize1D,nullptr, nullptr);
    waitFence(modelio);

    submit(modelio, VoxellstStage::directVNIR, voxelSize1D,nullptr, nullptr);
    waitFence(modelio);

    submit(modelio, VoxellstStage::diffuseVNIR, voxelSize1D,nullptr, nullptr);
    waitFence(modelio);

    submit(modelio, VoxellstStage::directTIR, voxelSize1D,nullptr, nullptr);
    waitFence(modelio);

    submit(modelio, VoxellstStage::diffuseTIR, voxelSize1D,nullptr, nullptr);
    waitFence(modelio);

    //--------------------------------------------------


    submit(modelio, VoxellstStage::aero, voxelSize1D,nullptr, nullptr);
    waitFence(modelio);

    //--------------------------------------------------

    submit(modelio, VoxellstStage::bio, voxelSize1D,nullptr, nullptr);
    waitFence(modelio);


    //--------------------------------------------------


    submit(modelio, VoxellstStage::evapo, voxelSize1D,nullptr, nullptr);
    waitFence(modelio);

    //--------------------------------------------------

    submit(modelio, VoxellstStage::budget, voxelSize1D,nullptr, nullptr);
    waitFence(modelio);


    //--------------------------------------------------





    return true;
}


//bool Command::run(std::shared_ptr<VoxellstIO> &modelio){
//
//    modelio->m_currentSemaphore = 1;
//
//    glm::ivec3 voxelSize1D = glm::ivec3((modelio->n_voxel + (GROUP_SIZEX - 1)) / GROUP_SIZEX, 1, 1);
//    auto & descSet = modelio->m_descSet;
//    auto & setting = modelio->setting;
//
//
//    auto & piplineLayout_rt = modelio->m_pipelineLayout_rt;
//    auto & piplines_rt = modelio->m_pipelines_rt;
//    auto & piplineLayout_eb = modelio->m_pipelineLayout_eb;
//    auto & piplines_eb = modelio->m_pipelines_eb;
//    auto & piplineLayout_et = modelio->m_pipelineLayout_et;
//    auto & piplines_et = modelio->m_pipelines_et;
//    auto & piplineLayout_bio = modelio->m_pipelineLayout_bio;
//    auto & pipline_bio = modelio->m_pipeline_bio;
//    auto & piplineLayout_aero = modelio->m_pipelineLayout_aero;
//    auto & pipline_aero = modelio->m_pipeline_aero;
//    //--------------------------------------------------
//
//    int stageInt = 0;
//
//    stageInt = (int)VoxelRadStage::gap;
//    submit(modelio, voxelSize1D,descSet,piplineLayout_rt,piplines_rt[stageInt],setting,nullptr, nullptr);
//    waitFence(modelio);
//
//    stageInt = (int)VoxelRadStage::directVNIR;
//    submit(modelio, voxelSize1D,descSet,piplineLayout_rt,piplines_rt[stageInt],setting,nullptr, nullptr);
//    waitFence(modelio);
//
//    stageInt = (int)VoxelRadStage::diffuseVNIR;
//    submit(modelio, voxelSize1D,descSet,piplineLayout_rt,piplines_rt[stageInt],setting,nullptr, nullptr);
//    waitFence(modelio);
//
//
//
//
//    //--------------------------------------------------
//
//
//    submit(modelio, voxelSize1D,descSet,piplineLayout_bio,pipline_bio,setting,nullptr, nullptr);
//    waitFence(modelio);
//
//    //--------------------------------------------------
//
//    submit(modelio, voxelSize1D,descSet,piplineLayout_aero,pipline_aero,setting,nullptr, nullptr);
//    waitFence(modelio);
//
//
//    //--------------------------------------------------
//
//
//    stageInt = (int)ETStage::evapo;
//    submit(modelio, voxelSize1D,descSet,piplineLayout_et,piplines_et[stageInt],setting,nullptr, nullptr);
//    waitFence(modelio);
//
//    //--------------------------------------------------
//
//    stageInt = (int)EBStage::budget;
//    submit(modelio, voxelSize1D,descSet,piplineLayout_eb,piplines_eb[stageInt],setting,nullptr, nullptr);
//    waitFence(modelio);
//
//
//    //--------------------------------------------------
//
//
//
//
//
//    return true;
//}


void Command::submit(std::shared_ptr<VoxellstIO> &modelio, VoxellstStage stage, glm::ivec3 dispatchSize,
                     const std::optional<VkSemaphore> &inSemaphore, const std::optional<VkSemaphore> &outSemaphore)
{

    auto & descSet = modelio->m_descSet;
    auto & setting = modelio->setting;
    auto & pipelineLayout = modelio->m_pipelineLayout;
    auto & pipeline = modelio->m_pipelines[stage];


    auto & m_currentSemaphore = modelio->m_currentSemaphore;
    // Preparing for the compute shader
    VkCommandBuffer cmdBuf = modelio->m_genCmdBuf.createCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmdBuf, &beginInfo);

    // Dispatching the shader only for the other;
    recordCommandBuffer(cmdBuf, descSet, pipelineLayout, pipeline, setting );
    vkCmdDispatch(cmdBuf, dispatchSize.x, dispatchSize.y, dispatchSize.z);

    // new semaphores
    std::array<VkPipelineStageFlags, 1> waitStages{VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT};
    VkSubmitInfo submitInfoCompute{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfoCompute.waitSemaphoreCount =  static_cast<uint32_t>(1);
    submitInfoCompute.pWaitSemaphores =  inSemaphore.has_value() ? &inSemaphore.value() : & modelio->m_semaphores[m_currentSemaphore - 1];
    submitInfoCompute.pWaitDstStageMask = waitStages.data();
    submitInfoCompute.commandBufferCount = 1;
    submitInfoCompute.pCommandBuffers = &cmdBuf;
    submitInfoCompute.signalSemaphoreCount = 1;
    submitInfoCompute.pSignalSemaphores = outSemaphore.has_value() ? &outSemaphore.value() : & modelio->m_semaphores[m_currentSemaphore];



    // old using only fense
//    const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
//    submitInfo.pWaitDstStageMask = &waitStageMask;
//    submitInfo.pCommandBuffers = &cmdBuf;
//    submitInfo.commandBufferCount = 1;

    vkEndCommandBuffer(cmdBuf);
    vkQueueSubmit( modelio->m_queue, 1, &submitInfoCompute,  modelio->m_fence);
    m_currentSemaphore++;
}


void Command::submit(std::shared_ptr<VoxellstIO> &modelio,glm::ivec3 dispatchSize,
                     VkDescriptorSet descSet, VkPipelineLayout pipelineLayout,
                     VkPipeline pipeline, VoxelLstSetting setting,
                     const std::optional<VkSemaphore> &inSemaphore, const std::optional<VkSemaphore> &outSemaphore)
{
    auto & m_currentSemaphore = modelio->m_currentSemaphore;
    // Preparing for the compute shader
    VkCommandBuffer cmdBuf = modelio->m_genCmdBuf.createCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmdBuf, &beginInfo);

    // Dispatching the shader only for the other;
    recordCommandBuffer(cmdBuf, descSet, pipelineLayout, pipeline, setting );
    vkCmdDispatch(cmdBuf, dispatchSize.x, dispatchSize.y, dispatchSize.z);

    // new semaphores
    std::array<VkPipelineStageFlags, 1> waitStages{VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT};
    VkSubmitInfo submitInfoCompute{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfoCompute.waitSemaphoreCount =  static_cast<uint32_t>(1);
    submitInfoCompute.pWaitSemaphores =  inSemaphore.has_value() ? &inSemaphore.value() : & modelio->m_semaphores[m_currentSemaphore - 1];
    submitInfoCompute.pWaitDstStageMask = waitStages.data();
    submitInfoCompute.commandBufferCount = 1;
    submitInfoCompute.pCommandBuffers = &cmdBuf;
    submitInfoCompute.signalSemaphoreCount = 1;
    submitInfoCompute.pSignalSemaphores = outSemaphore.has_value() ? &outSemaphore.value() : & modelio->m_semaphores[m_currentSemaphore];



    // old using only fense
//    const VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
//    submitInfo.pWaitDstStageMask = &waitStageMask;
//    submitInfo.pCommandBuffers = &cmdBuf;
//    submitInfo.commandBufferCount = 1;

    vkEndCommandBuffer(cmdBuf);
    vkQueueSubmit( modelio->m_queue, 1, &submitInfoCompute,  modelio->m_fence);
    m_currentSemaphore++;
}

void Command::recordCommandBuffer(VkCommandBuffer cmdBuf, VkDescriptorSet descSet, VkPipelineLayout pipelineLayout,
                     std::map<VoxelRadStage, VkPipeline> pipelines, VoxelRadStage stage, VoxelLstSetting setting)
{
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipelines[stage]);
    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0,
                            static_cast<uint32_t>(1), &descSet, 0, nullptr);
    // Sending the push constant information
    vkCmdPushConstants(cmdBuf, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(VoxelLstSetting), &setting);
}

void Command::recordCommandBuffer(VkCommandBuffer cmdBuf, VkDescriptorSet descSet, VkPipelineLayout pipelineLayout,
                                  VkPipeline pipeline,  VoxelLstSetting setting)
{
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0,
                            static_cast<uint32_t>(1), &descSet, 0, nullptr);
    // Sending the push constant information
    vkCmdPushConstants(cmdBuf, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(VoxelLstSetting), &setting);
}



void Command::waitFence(std::shared_ptr<VoxellstIO> &modelio)
{
    vkWaitForFences(modelio->m_device, 1, &(modelio->m_fence), VK_TRUE, UINT64_MAX);
    vkResetFences(modelio->m_device, 1,  &(modelio->m_fence));
}


void Command::destroy(std::shared_ptr<VoxellstIO> &modelio) {


    vkDestroyFence(modelio->m_device, modelio->m_fence, nullptr);
       // vkFreeCommandBuffers(m_device, m_cmdPool, 1, &m_commandBuffers[i]);

    for (auto& semaphore : modelio->m_semaphores)
    {
        vkDestroySemaphore(modelio->m_device, semaphore, nullptr);
    }

}
