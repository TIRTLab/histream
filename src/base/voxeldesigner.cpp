#include "voxeldesigner.h"
#include <nvh/gltfscene.cpp>
//#include "autogen/VoxelDesigner.comp.h"
// using BufferT = nanovdb::HostBuffer;

void VoxelDesigner::loadModel(ShapeType shapeType, Shape shape, float stepSize)
{
    // 填充m_voxelModel内部的aabbs
    switch (shapeType)
    {
    case ShapeType::CUBE:
        createCube(shape.length, shape.width, shape.height, stepSize);
        break;
    case ShapeType::ELLIPSOID:
        createEllipsoid(shape.height, shape.width, stepSize);
        break;
    case ShapeType::PLANE:
        createBackground(shape.length, shape.width, stepSize);
        break;
    default:
        createCube(shape.length, shape.width, shape.height, stepSize);
        break;
    }
}

// 均为xyz坐标
std::vector<glm::ivec3> VoxelDesigner::createEllipsoid(float height, float width, float stepSize) // Shape.length...
{
    std::vector<glm::ivec3> discreteCoord;
    std::vector<Aabb> aabbs;

    int widthSize = std::ceil(width / stepSize);
    int heightSize = std::ceil(height / stepSize);
    int lengthSize = std::ceil(width / stepSize);

    float widthCenter = widthSize / 2.0;
    float heightCenter = heightSize / 2.0;
    float lengthCenter = lengthSize / 2.0;

    for (int i = 0; i < lengthSize; i++)
    {
        for (int j = 0; j < widthSize; j++)
        {
            for (int k = 0; k < heightSize; k++)
            {
                // move to center of 1x1 voxel
                // (coord + 0.5) - (center + 0.5) = coord - center
                float dist = (i - lengthCenter) * (i - lengthCenter) / (lengthCenter * lengthCenter) +
                             (j - widthCenter) * (j - widthCenter) / (widthCenter * widthCenter) +
                             (k - heightCenter) * (k - heightCenter) / (heightCenter * heightCenter);
                if (dist <= 1.0)
                {
                    // ellipsoid.activedNum++;
                    glm::vec3 coord = {i, j, k};
                    Aabb aabb = { {coord},
                                 {coord + glm::vec3(1)}};
                    discreteCoord.emplace_back(coord);
                    aabbs.emplace_back(aabb);
                }
            }
        }
    }

    /*if (ellipsoid.activedNum < 1)
    {
        int point[3] = {int(widthCenter), int(widthCenter), int(widthCenter)};
        ellipsoid.voxels.emplace_back(point);
        ellipsoid.activedNum++;
    }*/

    m_voxelModel.centerPoints = discreteCoord;
    m_voxelModel.aabbs = aabbs;
    return discreteCoord;
}


std::vector<glm::ivec3> VoxelDesigner::createCube(float length, float width, float height, float stepSize)
{
    std::vector<glm::ivec3> discreteCoord;
    std::vector<Aabb> aabbs;

    int widthSize = std::ceil(width / stepSize);
    int heightSize = std::ceil(height / stepSize);
    int lengthSize = std::ceil(length / stepSize);

    float widthCenter = widthSize / 2.0;
    float heightCenter = heightSize / 2.0;
    float lengthCenter = lengthSize/ 2.0;

    // i,j为层，k为列
    for (int k = 0; k < heightSize; k++)
    {
    for (int i = 0; i < lengthSize; i++)
    {
        for (int j = 0; j < widthSize; j++)
        {
            
                glm::vec3 coord = {i, j, k};
                Aabb aabb = {{coord},
                             {coord + glm::vec3(1)}};
                discreteCoord.emplace_back(coord);
                aabbs.emplace_back(aabb);
            }
        }
    }

    m_voxelModel.centerPoints = discreteCoord;
    m_voxelModel.aabbs = aabbs;
    return discreteCoord;
}

PrimMesh VoxelDesigner::createTriCube(Shape shape, float stepSize)
{
    PrimMesh voxelTriModel;
    voxelTriModel.nVertices = 0;
    voxelTriModel.nIndices = 0;
    voxelTriModel.meshId = 0;

    // 绘制每一层x,y平面
    for (int k = 0; k < shape.height / stepSize; k++) //height,绘制每一层的面
    {
        for (int i = 0; i < shape.length / stepSize; i++)
        {
            for (int j = 0; j < shape.width / stepSize; j++)
            {
                glm::vec3 points[4];
                points[0].x = i;
                points[0].y = j;
                points[0].z = k;

                points[1].x = (i);
                points[1].y = (j + 1);
                points[1].z = k;

                points[2].x = (i + 1);
                points[2].y = (j + 1);
                points[2].z = k;

                points[3].x = (i + 1);
                points[3].y = (j);
                points[3].z = k;

                 int list[] = {0, 1, 2, 0, 2, 3};
                for (int kk = 0; kk < 6; kk++)
                {
                    int listK = list[kk];
                    VertexAttribute va{points[listK]};
                    voxelTriModel.vertices.emplace_back(va);

                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + kk);
                }

                voxelTriModel.nVertices += 6;
                voxelTriModel.nIndices += 6;
            }
        }
    }
    
    // 绘制每一层y,z平面
    for (int k = 0; k < shape.height / stepSize; k++) //height,绘制每一层的面
    {
        for (int i = 0; i < shape.length / stepSize; i++)
        {
            for (int j = 0; j < shape.width / stepSize; j++)
            {
                glm::vec3 points[4];
                points[0].x = i;
                points[0].y = j;
                points[0].z = k;

                points[1].x = i;
                points[1].y = (j + 1);
                points[1].z = k;

                points[2].x = i;
                points[2].y = (j + 1);
                points[2].z = (k + 1);

                points[3].x = i;
                points[3].y = j;
                points[3].z = (k + 1);

                 int list[] = {0, 1, 2, 0, 2, 3};
                for (int kk = 0; kk < 6; kk++)
                {
                    int listK = list[kk];
                    VertexAttribute va{points[listK]};
                    voxelTriModel.vertices.emplace_back(va);

                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + kk);
                }

                voxelTriModel.nVertices += 6;
                voxelTriModel.nIndices += 6;
            }
        }
    }
   
    // 绘制每一层x,z平面
    for (int k = 0; k < shape.height / stepSize; k++) //height,绘制每一层的面
    {
        for (int i = 0; i < shape.length / stepSize; i++)
        {
            for (int j = 0; j < shape.width / stepSize; j++)
            {
                glm::vec3 points[4];
                points[0].x = i;
                points[0].y = j;
                points[0].z = k;

                points[1].x = (i + 1);
                points[1].y = j;
                points[1].z = k;

                points[2].x = (i + 1);
                points[2].y = j;
                points[2].z = (k + 1);

                points[3].x = i;
                points[3].y = j ;
                points[3].z = (k + 1);

                int list[] = {0, 1, 2, 0, 2, 3};
                for (int kk = 0; kk < 6; kk++)
                {
                    int listK = list[kk];
                    VertexAttribute va{points[listK]};
                    voxelTriModel.vertices.emplace_back(va);

                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + kk);
                }

                voxelTriModel.nVertices += 6;
                voxelTriModel.nIndices += 6;
                voxelTriModel.centers.emplace_back(glm::vec3{i, j, k});
            }
        }
    }

    // 绘制顶面(x,y)
    for (int i = 0; i < shape.length / stepSize; i++)
    {
        for (int j = 0; j < shape.width / stepSize; j++)
        {
            glm::vec3 points[4];
            points[0].x = i;
            points[0].y = j ;
            points[0].z = shape.height ;

            points[1].x = (i);
            points[1].y = (j + 1);
            points[1].z = shape.height;

            points[2].x = (i + 1);
            points[2].y = (j + 1);
            points[2].z = shape.height;

            points[3].x = (i + 1);
            points[3].y = (j);
            points[3].z = shape.height;

            int list[] = {0, 1, 2, 0, 2, 3};
            for (int kk = 0; kk < 6; kk++)
            {
                int listK = list[kk];
                VertexAttribute va{points[listK]};
                voxelTriModel.vertices.emplace_back(va);

                voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + kk);
            }

            voxelTriModel.nVertices += 6;
            voxelTriModel.nIndices += 6;
        }
    }

    // 绘制y,z最大面
    for (int k = 0; k < shape.height / stepSize; k++) //height,绘制每一层的面
    {
            for (int j = 0; j < shape.width / stepSize; j++)
            {
                glm::vec3 points[4];
                points[0].x = shape.length;
                points[0].y = j;
                points[0].z = k;

                points[1].x = shape.length;
                points[1].y = (j + 1);
                points[1].z = k;

                points[2].x = shape.length;
                points[2].y = (j + 1);
                points[2].z = (k + 1);

                points[3].x = shape.length;
                points[3].y = j;
                points[3].z = (k + 1);

                int list[] = {0, 1, 2, 0, 2, 3};
                for (int kk = 0; kk < 6; kk++)
                {
                    int listK = list[kk];
                    VertexAttribute va{points[listK]};
                    voxelTriModel.vertices.emplace_back(va);

                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + kk);
                }

                voxelTriModel.nVertices += 6;
                voxelTriModel.nIndices += 6;
            }
    }

    // 绘制x,z最大面
    for (int k = 0; k < shape.height / stepSize; k++) //height,绘制每一层的面
    {
        for (int i = 0; i < shape.length / stepSize; i++)
        {
                glm::vec3 points[4];
                points[0].x = i;
                points[0].y = shape.width;
                points[0].z = k;

                points[1].x = (i + 1);
                points[1].y = shape.width;
                points[1].z = k;

                points[2].x = (i + 1);
                points[2].y = shape.width;
                points[2].z = (k + 1);

                points[3].x = i;
                points[3].y = shape.width;
                points[3].z = (k + 1);

                int list[] = {0, 1, 2, 0, 2, 3};
                for (int kk = 0; kk < 6; kk++)
                {
                    int listK = list[kk];
                    VertexAttribute va{points[listK]};
                    voxelTriModel.vertices.emplace_back(va);

                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + kk);
                }

                voxelTriModel.nVertices += 6;
                voxelTriModel.nIndices += 6;
        }
    }


    // 计算模型的x,y平面坐标下中心点位置
    /*int centerPointX, centerPointY, centerPointZ;
    centerPointX = int(shape.length / stepSize / 2.0);
    centerPointY = int(shape.width / stepSize / 2.0);
    centerPointZ = int(shape.height / stepSize / 2.0);
    centerPointZ = 0;
    voxelTriModel.modelCenterPoint = glm::ivec3(centerPointX, centerPointY, centerPointZ);*/
   voxelTriModel.meshcenter = glm::vec3(shape.length / 2.0, shape.width / 2.0, 0 / 2.0);

    return voxelTriModel;
}

PrimMesh VoxelDesigner::createTriEllipsoid(Shape shape, float stepSize)
{
    PrimMesh voxelTriModel;

    voxelTriModel.nVertices = 0;
    voxelTriModel.nIndices = 0;
    voxelTriModel.meshId = 0;

    int widthSize = std::ceil(shape.width / stepSize);
    int heightSize = std::ceil(shape.height / stepSize);
    int lengthSize = std::ceil(shape.length / stepSize);

    float widthCenter = (shape.width / stepSize) / 2.0;
    float heightCenter = (shape.height / stepSize) / 2.0;
    float lengthCenter = (shape.length / stepSize) / 2.0;

    for (int i = 0; i < lengthSize; i++)
    {
        for (int j = 0; j < widthSize; j++)
        {
            for (int k = 0; k < heightSize; k++)
            {
                float dist = (i - lengthCenter + 0.5) * (i - lengthCenter + 0.5) / (lengthCenter * lengthCenter) +
                             (j - widthCenter + 0.5) * (j - widthCenter + 0.5) / (widthCenter * widthCenter) +
                             (k - heightCenter + 0.5) * (k - heightCenter + 0.5) / (heightCenter * heightCenter);
                // left, forward and bottom face
                if (dist <= 1.0)
                {
                    glm::ivec3 voxel = glm::ivec3(i,j,k);
                    glm::vec3 point[7];

                    point[0].x = voxel.x * stepSize;
                    point[0].y = voxel.y * stepSize;
                    point[0].z = voxel.z * stepSize;

                    point[1].x = voxel.x * stepSize;
                    point[1].y = (voxel.y + 1) * stepSize;
                    point[1].z = voxel.z * stepSize;

                    point[2].x = (voxel.x + 1) * stepSize;
                    point[2].y = (voxel.y + 1) * stepSize;
                    point[2].z = voxel.z * stepSize;

                    point[3].x = (voxel.x + 1) * stepSize;
                    point[3].y = voxel.y * stepSize;
                    point[3].z = voxel.z * stepSize;

                    point[4].x = voxel.x * stepSize;
                    point[4].y = voxel.y * stepSize;
                    point[4].z = (voxel.z + 1) * stepSize;

                    point[5].x = voxel.x * stepSize;
                    point[5].y = (voxel.y + 1) * stepSize;
                    point[5].z = (voxel.z + 1) * stepSize;

                    point[6].x = (voxel.x + 1) * stepSize;
                    point[6].y = voxel.y * stepSize;
                    point[6].z = (voxel.z + 1) * stepSize;

                    for (int vi = 0; vi < 7; vi++)
                    {
                        VertexAttribute va;
                        va.pos = point[vi];
                        voxelTriModel.vertices.emplace_back(va);
                    }

                    // 0,1,2,3 bottom
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 1);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 2);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 2);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 3);

                    // 0,1,4,5 left
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 1);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 5);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 4);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 5);

                    // 0,3,4,6 forward
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 4);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 6);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 3);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 6);

                    voxelTriModel.nVertices += 7;
                    voxelTriModel.nIndices += 6 * 3;

                   voxelTriModel.centers.emplace_back(glm::vec3{i, j, k});
                }

                // up face
                float upDist = (i - lengthCenter + 0.5) * (i - lengthCenter + 0.5) / (lengthCenter * lengthCenter) +
                               (j - widthCenter + 0.5) * (j - widthCenter + 0.5) / (widthCenter * widthCenter) +
                               (k - heightCenter + 1.5) * (k - heightCenter + 1.5) / (heightCenter * heightCenter);
                if (dist <= 1.0 && upDist > 1.0)
                {
                    glm::ivec3 voxel = glm::ivec3(i, j, k);
                    glm::vec3 point[4];

                    point[0].x = voxel.x * stepSize;
                    point[0].y = voxel.y * stepSize;
                    point[0].z = (voxel.z + 1) * stepSize;

                    point[1].x = voxel.x * stepSize;
                    point[1].y = (voxel.y + 1) * stepSize;
                    point[1].z = (voxel.z + 1) * stepSize;

                    point[2].x = (voxel.x + 1) * stepSize;
                    point[2].y = (voxel.y + 1) * stepSize;
                    point[2].z = (voxel.z + 1) * stepSize;

                    point[3].x = (voxel.x + 1) * stepSize;
                    point[3].y = voxel.y * stepSize;
                    point[3].z = (voxel.z + 1) * stepSize;

                    for (int vi = 0; vi < 4; vi++)
                    {
                        VertexAttribute va;
                        va.pos = point[vi];
                        voxelTriModel.vertices.emplace_back(va);
                    }

                    // 0,1,2,3 up
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 1);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 2);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 2);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 3);

                    voxelTriModel.nVertices += 4;
                    voxelTriModel.nIndices += 2 * 3;
                }

                // backward face
                float backwardDist = (i - lengthCenter + 0.5) * (i - lengthCenter + 0.5) / (lengthCenter * lengthCenter) +
                                     (j - widthCenter + 1.5) * (j - widthCenter + 1.5) / (widthCenter * widthCenter) +
                                     (k - heightCenter + 0.5) * (k - heightCenter + 0.5) / (heightCenter * heightCenter);
                if (dist <= 1.0 && backwardDist > 1.0)
                {
                    glm::ivec3 voxel = glm::ivec3(i, j, k);
                    glm::vec3 point[4];

                    point[0].x = voxel.x * stepSize;
                    point[0].y = (voxel.y + 1) * stepSize;
                    point[0].z = voxel.z * stepSize;

                    point[1].x = voxel.x * stepSize;
                    point[1].y = (voxel.y + 1) * stepSize;
                    point[1].z = (voxel.z + 1) * stepSize;

                    point[2].x = (voxel.x + 1) * stepSize;
                    point[2].y = (voxel.y + 1) * stepSize;
                    point[2].z = (voxel.z + 1) * stepSize;

                    point[3].x = (voxel.x + 1) * stepSize;
                    point[3].y = (voxel.y + 1) * stepSize;
                    point[3].z = voxel.z * stepSize;

                    for (int vi = 0; vi < 4; vi++)
                    {
                        VertexAttribute va;
                        va.pos = point[vi];
                        voxelTriModel.vertices.emplace_back(va);
                    }

                    // 0,1,2,3 up
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 1);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 2);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 2);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 3);

                    voxelTriModel.nVertices += 4;
                    voxelTriModel.nIndices += 2 * 3;
                }

                // right face
                float rightDist = (i - lengthCenter + 1.5) * (i - lengthCenter + 1.5) / (lengthCenter * lengthCenter) +
                                  (j - widthCenter + 0.5) * (j - widthCenter + 0.5) / (widthCenter * widthCenter) +
                                  (k - heightCenter + 0.5) * (k - heightCenter + 0.5) / (heightCenter * heightCenter);
                if (dist <= 1.0 && rightDist > 1.0)
                {
                    glm::ivec3 voxel = glm::ivec3(i, j, k);
                    glm::vec3 point[4];

                    point[0].x = (voxel.x + 1) * stepSize;
                    point[0].y = voxel.y * stepSize;
                    point[0].z = voxel.z * stepSize;

                    point[1].x = (voxel.x + 1) * stepSize;
                    point[1].y = (voxel.y + 1) * stepSize;
                    point[1].z = voxel.z * stepSize;

                    point[2].x = (voxel.x + 1) * stepSize;
                    point[2].y = (voxel.y + 1) * stepSize;
                    point[2].z = (voxel.z + 1) * stepSize;

                    point[3].x = (voxel.x + 1) * stepSize;
                    point[3].y = voxel.y * stepSize;
                    point[3].z = (voxel.z + 1) * stepSize;

                    for (int vi = 0; vi < 4; vi++)
                    {
                        VertexAttribute va;
                        va.pos = point[vi];
                        voxelTriModel.vertices.emplace_back(va);
                    }

                    // 0,1,2,3 up
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 1);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 2);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 2);
                    voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + 3);

                    voxelTriModel.nVertices += 4;
                    voxelTriModel.nIndices += 2 * 3;
                }
            }
        }
    }

    //// 计算模型的x,y平面坐标下中心点位置
    //int centerPointX, centerPointY, centerPointZ;
    //centerPointX = int(shape.length / 2.0);
    //centerPointY = int(shape.width / 2.0);
    //centerPointZ = int(shape.height / 2.0);
    ////centerPointZ = 0;
    //voxelTriModel.modelCenterPoint = glm::ivec3(centerPointX, centerPointY, centerPointZ);

    voxelTriModel.meshcenter = glm::vec3(shape.length / 2.0, shape.width / 2.0, shape.height / 2.0);

    return voxelTriModel;
}

bool VoxelDesigner::isInPloy(nvmath::vec2i testPoint, std::vector<nvmath::vec2f> ploys)
{
    nvmath::vec2f A = ploys[0];
    nvmath::vec2f B = ploys[1];
    nvmath::vec2f C = ploys[2];
    nvmath::vec2f D = ploys[3];
    int y = testPoint[1];
    int x = testPoint[0];
    int a = (B.x - A.x) * (y - A.y) - (B.y - A.y) * (x - A.x);
    int b = (C.x - B.x) * (y - B.y) - (C.y - B.y) * (x - B.x);
    int c = (D.x - C.x) * (y - C.y) - (D.y - C.y) * (x - C.x);
    int d = (A.x - D.x) * (y - D.y) - (A.y - D.y) * (x - D.x);
    if ((a > 0 && b > 0 && c > 0 && d > 0) || (a < 0 && b < 0 && c < 0 && d < 0))
    {
        return true;
    }

    //		AB X AP = (b.x - a.x, b.y - a.y) x (p.x - a.x, p.y - a.y) = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
    //		BC X BP = (c.x - b.x, c.y - b.y) x (p.x - b.x, p.y - b.y) = (c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x);
    return false;
}



//Mesh VoxelDesigner::createTriCube_rotate(Shape shape, float stepSize, float rotateAngle)
//{
//  //  VoxelTriModel voxelTriModel;
//    Mesh mesh;
//    // 计算模型的x,y平面坐标下中心点位置,    会有空点
//    /*int centerPointX, centerPointY, centerPointZ;
//    centerPointX = int(shape.length / 2.0);
//    centerPointY = int(shape.width  / 2.0);
//    centerPointZ = int(shape.height / 2.0);*/
//    float centerPointX, centerPointY, centerPointZ;
//    centerPointX = (shape.length / 2.0);
//    centerPointY = (shape.width / 2.0);
//    centerPointZ = (shape.height / 2.0);
//    centerPointZ = 0;
//
//    glm::vec3 modelCenterPoint = glm::vec3(centerPointX, centerPointY, centerPointZ);
//    std::vector<glm::vec3> centerpoints;
//
//    float cosA = cos(rotateAngle * PI / 180);
//    float sinA = sin(rotateAngle * PI / 180);
//
//    // 计算旋转后的中心点位置， 以模型中心点为旋转中心
//    std::vector<glm::vec3> localCenterPoints;
//    for (int i=0; i < shape.length/ stepSize; i++)
//    {
//        for (int j = 0; j < shape.width/stepSize; j++)
//        {
//            for (int k = 0; k < shape.height/stepSize; k++)
//            {
//                // 全部保留
//                //localCenterPoints.push_back(glm::ivec3(i, j, k) - voxelTriModel.modelCenterPoint);
//                if (i == 0 || i == shape.length/stepSize -1)
//                {
//                    localCenterPoints.push_back(glm::vec3(i, j, k) - modelCenterPoint);
//                }
//                if (j == 0 || j==shape.width/stepSize - 1)
//                {
//                    localCenterPoints.push_back(glm::vec3(i, j, k) - modelCenterPoint);
//                }
//                /*if (k==0||k==shape.height/stepSize - 1)
//                {
//                    localCenterPoints.push_back(glm::ivec3(i, j, k) - voxelTriModel.modelCenterPoint);
//                }*/
//            }
//        }
//    }
//    for (auto &localCenterPoint : localCenterPoints)
//    {
//
//        float x = cosA * localCenterPoint.x - sinA * localCenterPoint.y + modelCenterPoint.x;
//        float y = sinA * localCenterPoint.x + cosA * localCenterPoint.y + modelCenterPoint.y;
//        centerpoints.push_back(glm::vec3(round(x) * stepSize, round(y) * stepSize, localCenterPoint.z * stepSize));
//    }
//
//    // 顶面和底面
//    // 计算四个角点，然后得到其包含的四边形内部的点
//    std::vector<nvmath::vec2f> poly;
//    nvmath::vec2f point1 = {cosA * (0 - modelCenterPoint.x) - sinA * (0 -modelCenterPoint.y) + modelCenterPoint.x,
//                            sinA * (0 - modelCenterPoint.x) + cosA * (0 - modelCenterPoint.y) + modelCenterPoint.y};
//    nvmath::vec2f point2 = {cosA * (shape.length / stepSize - modelCenterPoint.x) - sinA * (0 - modelCenterPoint.y) + modelCenterPoint.x,
//                            sinA * (shape.length / stepSize - modelCenterPoint.x) + cosA * (0 - modelCenterPoint.y) + modelCenterPoint.y};
//    nvmath::vec2f point3 = {cosA * (0 - modelCenterPoint.x) - sinA * (shape.width / stepSize - modelCenterPoint.y) + modelCenterPoint.x,
//                            sinA * (0 - modelCenterPoint.x) + cosA * (shape.width / stepSize - modelCenterPoint.y) + modelCenterPoint.y};
//    nvmath::vec2f point4 = {cosA * (shape.length / stepSize - modelCenterPoint.x) - sinA * (shape.width / stepSize - modelCenterPoint.y) + modelCenterPoint.x,
//                            sinA * (shape.length / stepSize - modelCenterPoint.x) + cosA * (shape.width / stepSize - modelCenterPoint.y) + modelCenterPoint.y};
//    poly.push_back(point1);
//    poly.push_back(point2);
//    poly.push_back(point4);
//    poly.push_back(point3);
//    int max_X = std::max({point1.x, point2.x, point3.x, point4.x});
//    int max_Y = std::max({point1.y, point2.y, point3.y, point4.y});
//    int min_X = std::min({point1.x, point2.x, point3.x, point4.x});
//    int min_Y = std::min({point1.y, point2.y, point3.y, point4.y});
//    for (int i = min_X; i < max_X; i++)
//    {
//        for (int j = min_Y; j < max_Y; j++)
//        {
//            nvmath::vec2i test = {i, j};
//            if (isInPloy(test, poly))
//            {
//                voxelTriModel.centerPoints.push_back(glm::vec3(i * stepSize, j * stepSize, 0 * stepSize));
//                voxelTriModel.centerPoints.push_back(glm::vec3(i * stepSize, j * stepSize, (shape.height / stepSize - 1) * stepSize));
//            }
//        }
//    }
//
//
//
//
//    // 绘制每个面
//    for (auto &centerPoint : voxelTriModel.centerPoints)
//    {
//        glm::vec3 points[8];
//        points[0] = glm::vec3(centerPoint.x, centerPoint.y, centerPoint.z);
//        points[1] = glm::vec3(centerPoint.x + stepSize, centerPoint.y, centerPoint.z);
//        points[2] = glm::vec3(centerPoint.x + stepSize, centerPoint.y + stepSize, centerPoint.z);
//        points[3] = glm::vec3(centerPoint.x, centerPoint.y + stepSize, centerPoint.z);
//        points[4] = glm::vec3(centerPoint.x, centerPoint.y, centerPoint.z + stepSize);
//        points[5] = glm::vec3(centerPoint.x + stepSize, centerPoint.y, centerPoint.z + stepSize);
//        points[6] = glm::vec3(centerPoint.x + stepSize, centerPoint.y + stepSize, centerPoint.z + stepSize);
//        points[7] = glm::vec3(centerPoint.x, centerPoint.y + stepSize, centerPoint.z);
//
//        int list[] = {0, 1, 4,
//                      1, 4, 5,
//                      0, 1, 2,
//                      0, 2, 3,
//                      4, 5, 6,
//                      4, 6, 7,
//                      2, 3, 6,
//                      3, 6, 7,
//                      0, 3, 4,
//                      3, 4, 7,
//                      1, 5, 6,
//                      1, 2, 6};
//
//        for (int kk = 0; kk < 36; kk++)
//        {
//            int listK = list[kk];
//            VertexAttribute va{points[listK]};
//            voxelTriModel.vertices.emplace_back(va);
//
//            voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + kk);
//        }
//
//        voxelTriModel.nVertices += 36;
//        voxelTriModel.nIndices += 36;
//    }
//
//    return voxelTriModel;
//}

std::vector<glm::ivec3> VoxelDesigner::createBackground(float sceneLength, float sceneWidth, float stepSize)
{
    std::vector<glm::ivec3> discreteCoord;
    std::vector<Aabb> aabbs;
    int lengthSize = sceneLength / stepSize;
    int widthSize = sceneWidth /stepSize;

    int a = 0;
    for (int j = 0; j < lengthSize; j++)
    {
        for (int k = 0; k < widthSize; k++)
        {
            glm::ivec3 coord = glm::ivec3(j, k, 0);
            Aabb aabb = {{glm::vec3(coord) },
                {glm::vec3(coord) + glm::vec3(1.0)}};
            discreteCoord.emplace_back(coord);
            aabbs.emplace_back(aabb);
        }
    }

//    m_voxelModel.centerPoints = discreteCoord;
//    m_voxelModel.aabbs = aabbs;
    return discreteCoord;
}

PrimMesh VoxelDesigner::createTriBackground(float sceneLength, float sceneWidth, float stepSize)
{
    PrimMesh mesh;
    mesh.meshId = 0;
    mesh.nVertices = 0;
    mesh.nIndices = 0;
    float bgZ = 0;

    for (int i = 0; i < sceneLength/stepSize; i++)
    {
        for (int j = 0; j < sceneWidth/stepSize; j++)
        {
            glm::vec3 points[4];
            points[0].x = i * stepSize;
            points[0].y = j * stepSize;
            points[0].z = bgZ * stepSize;

            points[1].x = (i) * stepSize;
            points[1].y = (j + 1) * stepSize;
            points[1].z = bgZ * stepSize;

            points[2].x = (i + 1) * stepSize;
            points[2].y = (j + 1) * stepSize;
            points[2].z = bgZ * stepSize;

            points[3].x = (i + 1) * stepSize;
            points[3].y = (j) * stepSize;
            points[3].z = bgZ * stepSize;

            int list[] = {0, 1, 2, 0, 2, 3};
            for (int k = 0;k < 6; k++)
            {
                int listK = list[k];
                VertexAttribute va{points[listK]};
                mesh.vertices.emplace_back(va);
                mesh.indices.emplace_back(mesh.nVertices + k);
            }

            mesh.nVertices += 6;
            mesh.nIndices += 6;
            mesh.centers.emplace_back(glm::vec3{i * stepSize, j * stepSize, bgZ * stepSize});
        }
    }

    return mesh;
}

PrimMesh VoxelDesigner::createTriEntity(ShapeType shapeType, Shape shape, float stepSize)
{
    PrimMesh voxelTriModel;

    if (shapeType == ShapeType::CUBE)
    {
        voxelTriModel = createTriCube(shape, stepSize);
    }
    if (shapeType == ShapeType::ELLIPSOID)
    {
        voxelTriModel = createTriEllipsoid(shape, stepSize);
    }

    return voxelTriModel;
}

//VoxelTriModel VoxelDesigner::createTriEntity_rotate(ShapeType shapeType, Shape shape, float stepSize, float rotateAngle)
//{
//    VoxelTriModel voxelTriModel;
//    if (shapeType == ShapeType::CUBE)
//    {
//        voxelTriModel = createTriCube_rotate(shape, stepSize, rotateAngle);
//    }
//    if (shapeType == ShapeType::ELLIPSOID)
//    {
//        voxelTriModel = createTriEllipsoid(shape, stepSize);
//    }
//    return voxelTriModel;
//}

//VoxelTriModel VoxelDesigner::createTriBackgroundFromDEM(const std::string &filename, glm::vec3 sceneSize, _2D::BilinearInterpolator<double> &interp)
//{
//    VoxelTriModel voxelTriModel;
//    std::string infile = filename;
//    GDALAllRegister();
//
//    BYTE *pafScanblock;
//    GDALDataset *poDataset;
//    poDataset = (GDALDataset *)GDALOpen(infile.c_str(), GA_ReadOnly);
//    if (poDataset == NULL)
//    {
//        std::cout << "fail in open files!" << std::endl;
//        return voxelTriModel;
//    }
//    int nImgSizeX = poDataset->GetRasterXSize();
//    int nImgSizeY = poDataset->GetRasterYSize();
//    int bandcount = poDataset->GetRasterCount();
//
//    int num_image_size = 0;
//
//    FLOAT *pafScanline = new FLOAT[nImgSizeX * nImgSizeY];
//    poDataset->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanline, nImgSizeX, nImgSizeY, GDT_Float32, bandcount, 0, 0, 0, 0);
//
//    m_scale = sceneSize.x / nImgSizeX;
//    float scale = m_scale;
//
//    std::vector<double> x, y, z;
//    for (int i = 0; i < nImgSizeX; i++)
//    {
//        for (int j = 0; j < nImgSizeY; j++)
//        {
//            x.emplace_back((i * scale + scale / 2.0)); // pixel center location
//            y.emplace_back((j * scale + scale / 2.0));
//            z.emplace_back(pafScanline[i * nImgSizeY + j]);
//        }
//    }
//
//
//    interp.setData(x, y, z);
//
//    delete[] pafScanline;
//    delete poDataset;
//
//    float step = 1; // / 2.0; // control DEM subdivision degree, The smaller the value, the finer the distinction
//
//    //找寻voxel中心点,后加上中心点包含的体
//    uint32_t nVertices = 0;
//    uint32_t nIndices = 0;
//    minElevation = z[0];
//    for(int j=0; j < sceneSize.x; j=j+step)
//    {
//        for (int k=0; k<sceneSize.y; k=k+step)
//        {
//            float jj = j, kk= k;
//            if (jj < scale / 2)
//                jj = scale / 2.0;
//            if (kk < scale / 2)
//                kk = scale / 2.0;
//            if (jj > sceneSize.x - scale / 2.0)
//                jj = sceneSize.x - scale / 2.0 - 0.01;
//            if (kk > sceneSize.y - scale / 2.0)
//                kk = sceneSize.y - scale / 2.0 - 0.01;
//            voxelTriModel.centerPoints.emplace_back(glm::vec3{j + 0.5, k + 0.5,
//                                                                  int(interp(jj, kk))});
//
//            
//            glm::ivec3 voxel = glm::ivec3(j, k, int(interp(jj, kk)));
//
//            if (voxel.z < minElevation)
//                minElevation = voxel.z;
//            glm::vec3 point[8];
//            int stepSize = 1;
//            point[0].x = voxel.x * stepSize;
//            point[0].y = voxel.y * stepSize;
//            point[0].z = voxel.z * stepSize;
//
//            point[1].x = voxel.x * stepSize;
//            point[1].y = (voxel.y + 1) * stepSize;
//            point[1].z = voxel.z * stepSize;
//
//            point[2].x = (voxel.x + 1) * stepSize;
//            point[2].y = (voxel.y + 1) * stepSize;
//            point[2].z = voxel.z * stepSize;
//
//            point[3].x = (voxel.x + 1) * stepSize;
//            point[3].y = voxel.y * stepSize;
//            point[3].z = voxel.z * stepSize;
//
//            point[4].x = voxel.x * stepSize;
//            point[4].y = voxel.y * stepSize;
//            point[4].z = (voxel.z + 1) * stepSize;
//
//            point[5].x = voxel.x * stepSize;
//            point[5].y = (voxel.y + 1) * stepSize;
//            point[5].z = (voxel.z + 1) * stepSize;
//
//            point[6].x = (voxel.x + 1) * stepSize;
//            point[6].y = voxel.y * stepSize;
//            point[6].z = (voxel.z + 1) * stepSize;
//
//            point[7].x = (voxel.x + 1) * stepSize;
//            point[7].y = (voxel.y + 1) * stepSize;
//            point[7].z = (voxel.z + 1) * stepSize;
//
//            int list[] = {0, 1, 2, 0, 2, 3,
//                          0, 1, 5, 0, 4, 5,
//                          1, 2, 6, 1, 5, 6,
//                          4, 5, 6, 4, 6, 7,
//                          2, 3, 6, 3, 6, 7,
//                          0, 3, 7, 0, 4, 7};
//
//
//            //顶点
//            for (int kkk = 0; kkk < 36; kkk++)
//            {
//                int listK = list[kkk];
//                VertexAttribute va{point[listK]};
//                voxelTriModel.vertices.emplace_back(va);
//
//                voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + kkk);
//
//
//                //VertexAttribute va{point[kkk]};
//                //voxelTriModel.vertices.emplace_back(va);
//            }
//            // 边
//            /*for (int kkk = 0; kkk < 36; kkk++)
//            {
//                int listK = list[kkk];
//
//                voxelTriModel.indices.emplace_back(voxelTriModel.nVertices + listK);
//            }*/
//            voxelTriModel.nVertices += 36;
//            voxelTriModel.nIndices += 36;
//        }
//    }
//
//    return voxelTriModel;
//}

