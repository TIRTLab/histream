//
// Created by Fan Tengyuan on 9/12/2025.
//
#include "descriptor_voxelization.h"

bool Descriptor_voxelization::create(std::shared_ptr<VoxelizationIO> &modelio) {

    auto &bindings = modelio->m_bindings; // nvvk::DescriptorSetBindings 辅助类
    auto &m_device = modelio->m_device;

    // ----------------------------------------------------------------
    // 1. 定义 Binding Layout
    // ----------------------------------------------------------------
    // 这里的 binding 索引 (0) 必须与 Shader 中的 layout(binding = 0) 一致
    // 类型: STORAGE_IMAGE (用于 imageAtomicMax)
    // 阶段: FRAGMENT_BIT (因为我们在 FS 中写入)
    bindings.clear(); // 这是一个好习惯，防止多次调用时残留旧数据
    bindings.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_FRAGMENT_BIT);

    // ----------------------------------------------------------------
    // 2. 创建 Descriptor Set Layout 和 Pool
    // ----------------------------------------------------------------
    // 利用 nvvk 辅助类直接创建
    modelio->m_descSetLayout = bindings.createLayout(m_device);
    modelio->m_descPool      = bindings.createPool(m_device, 1);

    // ----------------------------------------------------------------
    // 3. 分配 Descriptor Set
    // ----------------------------------------------------------------
    modelio->m_descSet = nvvk::allocateDescriptorSet(m_device, modelio->m_descPool, modelio->m_descSetLayout);

    // ----------------------------------------------------------------
    // 4. 更新 Descriptor Set (写入具体资源)
    // ----------------------------------------------------------------
    std::vector<VkWriteDescriptorSet> updates;

    // 获取 Buffer 阶段创建好的 Texture Descriptor Info
    VkDescriptorImageInfo imageInfo = modelio->m_voxelTexture.descriptor;

    // 【关键】：强制设置 Layout 为 GENERAL
    // 虽然我们在 Buffer::create 里可能已经设置过了，但为了双重保险，
    // 在 WriteDescriptorSet 中必须明确指定它是以 GENERAL 布局被 Shader 访问的。
    // 如果是 SHADER_READ_ONLY_OPTIMAL，写入操作会未定义行为。
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    // 生成 Write 结构体
    updates.emplace_back(bindings.makeWrite(modelio->m_descSet, 0, &imageInfo));

    // 提交更新到 GPU
    vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(updates.size()), updates.data(), 0, nullptr);

    return false;
}

void Descriptor_voxelization::destroy(std::shared_ptr<VoxelizationIO>& modelio)
{
    // 销毁资源
    vkDestroyDescriptorPool(modelio->m_device, modelio->m_descPool, nullptr);
    vkDestroyDescriptorSetLayout(modelio->m_device, modelio->m_descSetLayout, nullptr);

    // 置空句柄防止悬空
    modelio->m_descPool = VK_NULL_HANDLE;
    modelio->m_descSetLayout = VK_NULL_HANDLE;
    modelio->m_descSet = VK_NULL_HANDLE;
}

