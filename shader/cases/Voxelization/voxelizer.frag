#version 450
// 必须声明支持原子操作的格式
layout(set = 0, binding = 0, r32ui) uniform uimage3D voxelGrid;

layout(location = 0) in vec3 gWorldPos;
layout(location = 1) flat in uint gObjectID;

void main() {
    // 1. 离散化坐标
    ivec3 voxelPos = ivec3(gWorldPos);
    
    // 2. 边界保护 (防止写出界)
    ivec3 size = imageSize(voxelGrid);
    if (any(lessThan(voxelPos, ivec3(0))) || any(greaterThanEqual(voxelPos, size))) {
        discard;
    }

    // 3. 计算权重 (距离中心越近权重越大)
    vec3 center = vec3(voxelPos) + 0.5;
    float distSq = dot(gWorldPos - center, gWorldPos - center);
    uint weight = uint((1.0 - clamp(distSq, 0.0, 1.0)) * 255.0); // 简单权重

    // 4. 打包: 高16位是权重, 低16位是ID
    uint packedVal = (weight << 16) | (gObjectID & 0xFFFF);

    // 5. 原子写入
    imageAtomicMax(voxelGrid, voxelPos, packedVal);
    
    // 注意：这里没有 out color，因为不写 Framebuffer
}