#version 450
layout(location = 0) in vec3 inPosition; 
// 如果你有法线或UV，在这里添加，虽然体素化通常只需要位置

// Push Constant 必须与 C++ struct PushConstantVoxel 内存布局严格一致
layout(push_constant) uniform PushConsts {
    mat4 modelMatrix;   // [64 bytes]
    uint objectID;      // [4 bytes]
    float padding[3];   // [12 bytes]
    vec4 volumeInfo;    // [16 bytes] .x = maxDim (网格最大边长)
} pco;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out flat uint outObjectID;

void main() {
    // 1. 计算世界坐标
    vec4 worldPos = pco.modelMatrix * vec4(inPosition, 1.0);
    
    // 2. 传递给 Geometry Shader
    outWorldPos = worldPos.xyz;
    outObjectID = pco.objectID;
    
    // 这里赋值 gl_Position 只是为了占位，实际坐标由 GS 决定
    gl_Position = worldPos; 
}