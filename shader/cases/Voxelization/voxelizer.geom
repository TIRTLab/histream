#version 450
layout(triangles) in;
// 输出 Triangle Strip，每个三角形输出一个投影后的三角形
layout(triangle_strip, max_vertices = 3) out;

// 输入来自 Vertex Shader
layout(location = 0) in vec3 inWorldPos[];
layout(location = 1) in flat uint inObjectID[];

// 输出给 Fragment Shader
layout(location = 0) out vec3 gWorldPos;
layout(location = 1) out flat uint gObjectID;

layout(push_constant) uniform PushConsts {
    mat4 modelMatrix;
    uint objectID;
    float padding[3];
    vec4 volumeInfo; // .x = maxDim (场景最大尺寸，如 1000.0)
} pco;

void main() {
    // 1. 获取三角形三个顶点
    vec3 p0 = inWorldPos[0];
    vec3 p1 = inWorldPos[1];
    vec3 p2 = inWorldPos[2];

    // 2. 计算面法线 (用于选择主轴)
    vec3 faceNormal = abs(cross(p1 - p0, p2 - p0));

    // 3. 寻找主轴 (Dominant Axis)
    // 目标是找到投影面积最大的平面，防止细长三角形漏掉
    uint axis = 0; 
    float maxNormal = faceNormal.x;

    if (faceNormal.y > maxNormal) {
        maxNormal = faceNormal.y;
        axis = 1;
    }
    if (faceNormal.z > maxNormal) {
        maxNormal = faceNormal.z;
        axis = 2;
    }

    // 4. 遍历三个顶点进行投影
    for(int i = 0; i < 3; i++) {
        vec3 pos = inWorldPos[i];
        
        // 传递真实的世界坐标给 Fragment Shader 用于计算体素索引
        gWorldPos = pos;
        gObjectID = inObjectID[i];

        // --- 投影逻辑 ---
        vec2 screenPos;
        
        // Swizzle: 把 3D 压扁成 2D
        if (axis == 0) {
            // X轴主导 -> 投影到 YZ 平面
            screenPos = pos.yz; 
        } else if (axis == 1) {
            // Y轴主导 -> 投影到 XZ 平面
            screenPos = pos.xz;
        } else {
            // Z轴主导 -> 投影到 XY 平面
            screenPos = pos.xy;
        }

        // --- 关键修正：坐标归一化 (World -> NDC) ---
        // 假设世界坐标范围是 [0, maxDim]
        // 我们需要将其映射到 NDC 的 [-1, 1]
        
        float maxDim = pco.volumeInfo.x;
        
        // 1. 归一化到 [0, 1]
        vec2 ndcPos = screenPos / maxDim;
        
        // 2. 映射到 [-1, 1]
        ndcPos = ndcPos * 2.0 - 1.0; 

        // 写入 gl_Position
        // z=0.0, w=1.0 保证在近裁剪面和远裁剪面之间
        gl_Position = vec4(ndcPos, 0.0, 1.0);
        
        EmitVertex();
    }
    EndPrimitive();
}