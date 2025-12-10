#version 450
// 扩展：支持 Shader 内存原子操作
#extension GL_EXT_shader_image_load_store : enable

// 绑定 3D 纹理 (必须声明 r32ui)
layout(set = 0, binding = 0, r32ui) uniform uimage3D voxelGrid;

layout(location = 0) in vec3 gWorldPos;
layout(location = 1) in flat uint gObjectID;

layout(push_constant) uniform PushConsts {
    mat4 modelMatrix;
    uint objectID;
    float padding[3];
    vec4 volumeInfo; // .x = maxDim
} pco;

void main() {
    // 1. 将世界坐标转换为体素整数坐标
    // 假设 1 unit = 1 voxel
    ivec3 voxelPos = ivec3(gWorldPos);
    
    // 2. 获取纹理尺寸进行边界检查
    ivec3 gridSize = imageSize(voxelGrid);

    // 3. 边界剔除 (非常重要，防止越界写入导致 Device Lost)
    if (any(lessThan(voxelPos, ivec3(0))) || any(greaterThanEqual(voxelPos, gridSize))) {
        // 如果你的模型原点不是 (0,0,0)，比如是中心在 (0,0,0) 的，
        // 你可能需要在上面的 voxelPos 计算时加上 offset:
        // ivec3 voxelPos = ivec3(gWorldPos + gridSize / 2);
        return; // Discard
    }

    // 4. 计算权重 (简单的中心距离权重)
    // 距离体素中心越近，权重越高 (0~255)
    vec3 voxelCenter = vec3(voxelPos) + 0.5;
    float distSq = dot(gWorldPos - voxelCenter, gWorldPos - voxelCenter);
    // 距离范围大约是 0 ~ 0.75 (对角线一半的平方)
    // 1.0 - distSq 越大越好
    uint weight = uint(clamp(1.0 - distSq, 0.0, 1.0) * 255.0);
    
    // 保证 ID 有效 (如果 ID=0 通常代表空)
    uint id = gObjectID & 0xFFFF; // 取低16位安全

    // 5. 数据打包: [ 高16位: Weight | 低16位: ID ]
    uint packedData = (weight << 16) | id;

    // 6. 原子写入 (保留权重最大的那个物体)
    imageAtomicMax(voxelGrid, voxelPos, packedData);
}