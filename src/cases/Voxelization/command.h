//
// Created by admin on 8/12/2025.
//

#ifndef STREAM_VK_COMMAND_H
#define STREAM_VK_COMMAND_H
#include "voxelizationio.h"

class Command {
public:
    Command();

    bool create(std::shared_ptr<VoxelizationIO> &modelio);

    bool run(std::shared_ptr<VoxelizationIO> &modelio);

    void submit(std::shared_ptr<VoxelizationIO> &modelio,
                const std::optional<VkSemaphore> &inSemaphore,
                const std::optional<VkSemaphore> &outSemaphore);

    void recordCommandBuffer(const VkCommandBuffer& cmdBuf,
                             std::shared_ptr<VoxelizationIO> &modelio);
    void waitFence(std::shared_ptr<VoxelizationIO> &modelio);
    void destroy(std::shared_ptr<VoxelizationIO> &modelio);



};
#endif //STREAM_VK_COMMAND_H
