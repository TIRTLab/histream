#version 450
layout(triangles) in;
// 输出 Triangle Strip，每个输入三角形只输出一个投影后的三角形
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 inWorldPos[];
layout(location = 1) in uint inObjectID[];

layout(location = 0) out vec3 gWorldPos;
layout(location = 1) flat out uint gObjectID; // flat 插值，ID 不需要平滑

layout(push_constant) uniform PushConsts {
    mat4 modelMatrix;
    uint objectID;
    vec3 volumeSize; // (1000.0, 1000.0, 500.0)
} pco;

void main() {
    // 1. 计算面法线 (寻找最大投影面)
    vec3 p0 = inWorldPos[0];
    vec3 p1 = inWorldPos[1];
    vec3 p2 = inWorldPos[2];
    vec3 normal = abs(cross(p1 - p0, p2 - p0));

    // 2. 确定主轴: 0=X(YZ面), 1=Y(XZ面), 2=Z(XY面)
    uint axis = 2; // 默认 XY
    if (normal.x > normal.y && normal.x > normal.z) axis = 0;
    else if (normal.y > normal.x && normal.y > normal.z) axis = 1;

    // 3. 处理三个顶点
    for(int i = 0; i < 3; i++) {
        vec3 pos = inWorldPos[i];
        
        // 传递给 FS 的必须是真实世界坐标
        gWorldPos = pos;
        gObjectID = inObjectID[i];

        // 投影逻辑：将 3D 坐标“压扁”到 2D 屏幕空间 (-1.0 ~ 1.0)
        vec2 screenPos;
        
        if (axis == 0) {
            // 投影到 YZ 平面
            screenPos = pos.yz; 
        } else if (axis == 1) {
            // 投影到 XZ 平面
            screenPos = pos.xz;
        } else {
            // 投影到 XY 平面
            screenPos = pos.xy;
        }

        // 坐标归一化 (从 0~1000 映射到 -1~1)
        // 这一步非常重要，你需要根据当前轴的最大尺寸进行缩放
        // 简单起见，这里假设 volumeSize 的最大边长决定了视口大小
        float maxDim = max(pco.volumeSize.x, max(pco.volumeSize.y, pco.volumeSize.z));
        
        screenPos /= maxDim; // 变到 0~1
        screenPos = screenPos * 2.0 - 1.0; // 变到 -1~1

        gl_Position = vec4(screenPos, 0.0, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}