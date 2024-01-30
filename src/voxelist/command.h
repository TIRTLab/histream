//
// Created by admin on 2024/1/26.
//

#ifndef FIELD_COMMAND_H
#define FIELD_COMMAND_H

#include "voxellstio.h"

class Command {
public:
    Command();

    bool create(std::shared_ptr<VoxellstIO> &VoxellstIO);
    bool run(std::shared_ptr<VoxellstIO> &VoxellstIO);

    void submit(std::shared_ptr<VoxellstIO> &VoxellstIO,glm::ivec3 dispatchSize,
                VkDescriptorSet descSet, VkPipelineLayout pipelineLayout,
                std::map<VoxelRadStage, VkPipeline> pipelines, VoxelRadStage stage,VoxelLstSetting setting,
                const std::optional<VkSemaphore> &inSemaphore, const std::optional<VkSemaphore> &outSemaphore);

    void submit(std::shared_ptr<VoxellstIO> &VoxellstIO, VoxellstStage stage, glm::ivec3 dispatchSize,
                const std::optional<VkSemaphore> &inSemaphore, const std::optional<VkSemaphore> &outSemaphore);


    void submit(std::shared_ptr<VoxellstIO> &VoxellstIO,glm::ivec3 dispatchSize,
                VkDescriptorSet descSet, VkPipelineLayout pipelineLayout,
                VkPipeline pipeline, VoxelLstSetting setting,
                const std::optional<VkSemaphore> &inSemaphore, const std::optional<VkSemaphore> &outSemaphore);

    void Command::recordCommandBuffer(VkCommandBuffer cmdBuf,
                                      VkDescriptorSet descSet, VkPipelineLayout pipelineLayout,
                                      VkPipeline pipeline,  VoxelLstSetting setting);

    void Command::recordCommandBuffer(VkCommandBuffer cmdBuf, VkDescriptorSet descSet, VkPipelineLayout pipelineLayout,
                                      std::map<VoxelRadStage, VkPipeline> pipelines, VoxelRadStage stage, VoxelLstSetting setting);

    void recordCommandBuffer(const VkCommandBuffer& cmdBuf,std::shared_ptr<VoxellstIO> &VoxellstIO);
    void waitFence(std::shared_ptr<VoxellstIO> &VoxellstIO);
    void destroy(std::shared_ptr<VoxellstIO> &VoxellstIO);

};


#endif //FIELD_COMMAND_H
