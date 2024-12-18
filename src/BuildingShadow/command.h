//
// Created by ShIP on 2024/12/2.
//


#ifndef FIELD_COMMAND_H
#define FIELD_COMMAND_H

#include "buildingshadowio.h"

class Command {
public:
    Command();

    bool create(std::shared_ptr<BuildingShadowIO> &modelio);

    bool run(std::shared_ptr<BuildingShadowIO> &modelio);

    // void submit(std::shared_ptr<BuildingShadowIO> &modelio,
    //             const std::optional<VkSemaphore> &inSemaphore,
    //             const std::optional<VkSemaphore> &outSemaphore);
    void submit(std::shared_ptr<BuildingShadowIO> &modelio, BuildingShadowStage stage, glm::ivec3 dispatchSize,
            const std::optional<std::vector<VkSemaphore>> &inSemaphore, const std::optional<VkSemaphore> &outSemaphore);

    // void recordCommandBuffer(const VkCommandBuffer& cmdBuf,std::shared_ptr<BuildingShadowIO> &modelio);
    void recordCommandBuffer(VkCommandBuffer cmdBuf,
                                          VkDescriptorSet descSet, VkPipelineLayout pipelineLayout,
                                          VkPipeline pipeline,  BuildingShadowSetting setting);


    void waitFence(std::shared_ptr<BuildingShadowIO> &modelio);

    void destroy(std::shared_ptr<BuildingShadowIO> &modelio);



};


#endif //COMMAND_H
