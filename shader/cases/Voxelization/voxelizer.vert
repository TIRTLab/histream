#version 450

// 假设你的顶点 Buffer 格式
layout(location = 0) in vec3 inPosition;
// 假设你通过 Instance 或者其他方式传入了物体 ID
layout(push_constant) uniform PushConsts {
    mat4 modelMatrix; // 模型矩阵
    uint objectID;    // 物体 ID
    vec3 volumeSize;  // 体素场景大小，如 vec3(1000, 1000, 500)
} pco;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out uint outObjectID;

void main() {
    // 变换到世界空间
    vec4 worldPos = pco.modelMatrix * vec4(inPosition, 1.0);
    outWorldPos = worldPos.xyz;
    outObjectID = pco.objectID;
    
    // 注意：这里不赋值 gl_Position，留给 GS 做
}