/******************************************************************************
 * Copyright 1998-2018 NVIDIA Corp. All Rights Reserved.
 *****************************************************************************/

// This file exist only to do the implementation of tiny obj loader
#define TINYOBJLOADER_IMPLEMENTATION
#include "objloader.h"
#include "nvh/nvprint.hpp"
#include <opencv2/opencv.hpp>
//#include "Interpolate.hpp"


//-----------------------------------------------------------------------------
// Extract the directory component from a complete path.
//
#ifdef WIN32
#define CORRECT_PATH_SEP "\\"
#define WRONG_PATH_SEP '/'
#else
#define CORRECT_PATH_SEP "/"
#define WRONG_PATH_SEP '\\'
#endif

/// <summary>
/// 把obj文件中所有的mesh都读取进去，只读取vertex和index
/// </summary>
/// <param name="filename"></param>
void ObjLoader::loadModel(const std::string& filename)
{
    clearCurrentInfo();

    tinyobj::ObjReader reader;
    reader.ParseFromFile(filename);
    if (!reader.Valid())
    {
        std::cerr << "Cannot load: " << filename << std::endl;
        assert(reader.Valid());
    }

    // 对内的所有的材质信息都不读取，只读取结构信息
    // Collecting the material in the scene
    //for(const auto& material : reader.GetMaterials())
    //{
    //  MaterialObj m;
    //  m.ambient  = nvmath::vec3f(material.ambient[0], material.ambient[1], material.ambient[2]);
    //  m.diffuse  = nvmath::vec3f(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
    //  m.specular = nvmath::vec3f(material.specular[0], material.specular[1], material.specular[2]);
    //  m.emission = nvmath::vec3f(material.emission[0], material.emission[1], material.emission[2]);
    //  m.transmittance = nvmath::vec3f(material.transmittance[0], material.transmittance[1],
    //                                  material.transmittance[2]);
    //  m.dissolve      = material.dissolve;
    //  m.ior           = material.ior;
    //  m.shininess     = material.shininess;
    //  m.illum          = material.illum;
    //  if(!material.diffuse_texname.empty())
    //  {
    //    m_textures.push_back(material.diffuse_texname);
    //    m.textureID = static_cast<int>(m_textures.size()) - 1;
    //  }
    //  m_materials.emplace_back(m);
    //}
    //// If there were none, add a default
    //if(m_materials.empty())
    //  m_materials.emplace_back(MaterialObj());

    const tinyobj::attrib_t& attrib = reader.GetAttrib();

    minElevation = attrib.vertices[1];
    for (const auto& shape : reader.GetShapes())
    {

        ShapeInfo     shapeInfo = {};
        shapeInfo.offset = m_indices.size();
        shapeInfo.nbFacet = shape.mesh.num_face_vertices.size();
        shapeInfo.matIndex = shape.mesh.material_ids[0];
        shapeInfo.name = shape.name;
        m_shapeInfo.emplace_back(shapeInfo);

        m_vertices.reserve(shape.mesh.indices.size() + m_vertices.size());
        m_indices.reserve(shape.mesh.indices.size() + m_indices.size());

        for (const auto& index : shape.mesh.indices)
        {
            VertexAttribute    vertex = {};

            const float* vp = &attrib.vertices[3 * index.vertex_index];
            vertex.pos = { (-1.0) * (*(vp + 0)), *(vp + 1), *(vp + 2) * (-1) };

            if (minElevation > vertex.pos.y) minElevation = vertex.pos.y;

            /* if(!attrib.normals.empty() && index.normal_index >= 0)
             {
               const float* np = &attrib.normals[3 * index.normal_index];
               vertex.nrm      = {*(np + 0), *(np + 1), *(np + 2)};
             }

             if(!attrib.texcoords.empty() && index.texcoord_index >= 0)
             {
               const float* tp = &attrib.texcoords[2 * index.texcoord_index + 0];
               vertex.texCoord = {*tp, 1.0f - *(tp + 1)};
             }*/

             /*if(!attrib.colors.empty())
             {
               const float* vc = &attrib.colors[3 * index.vertex_index];
               vertex.color    = {*(vc + 0), *(vc + 1), *(vc + 2)};
             }*/

            m_vertices.push_back(vertex);
            m_indices.push_back(static_cast<int>(m_indices.size()));
        }
    }
    m_objmesh.nVertices = m_vertices.size();
    m_objmesh.nIndices = m_indices.size();
    m_objmesh.vertices = m_vertices;
    m_objmesh.indices = m_indices;

    // Fixing material indices
    //for(auto& mi : m_matIndx)
    //{
    //  if(mi < 0 || mi > m_materials.size())
    //    mi = 0;
    //}


    // 如果没有法线，则计算法线
    //if(attrib.normals.empty())
    //{
    //  for(size_t i = 0; i < m_indices.size(); i += 3)
    //  {
    //    VertexObj& v0 = m_vertices[m_indices[i + 0]];
    //    VertexObj& v1 = m_vertices[m_indices[i + 1]];
    //    VertexObj& v2 = m_vertices[m_indices[i + 2]];

    //    nvmath::vec3f n = nvmath::normalize(nvmath::cross((v1.pos - v0.pos), (v2.pos - v0.pos)));
    //    v0.nrm          = n;
    //    v1.nrm          = n;
    //    v2.nrm          = n;
    //  }
    //}
}



/// <summary>
/// 把obj文件中特定mesh名字的读取进去，只读取vertex和index
/// </summary>
/// <param name="filename"></param>
/// <param name="meshname"></param>
void ObjLoader::loadMesh(const std::string& filename, const std::string& meshname)
{
    clearCurrentInfo();

    tinyobj::ObjReader reader;
    reader.ParseFromFile(filename);
    if (!reader.Valid())
    {
        std::cerr << "Cannot load: " << filename << std::endl;
        assert(reader.Valid());
    }

    const tinyobj::attrib_t& attrib = reader.GetAttrib();

    for (const auto& shape : reader.GetShapes())
    {

        if(shape.name != meshname) continue;
        ShapeInfo     shapeInfo = {};
        shapeInfo.offset = m_indices.size();
        shapeInfo.nbFacet = shape.mesh.num_face_vertices.size();
        shapeInfo.matIndex = shape.mesh.material_ids[0];
        shapeInfo.name = shape.name;
        m_shapeInfo.emplace_back(shapeInfo);

        m_vertices.reserve(shape.mesh.indices.size() + m_vertices.size());
        m_indices.reserve(shape.mesh.indices.size() + m_indices.size());

        for (const auto& index : shape.mesh.indices)
        {
            VertexAttribute    vertex = {};

            const float* vp = &attrib.vertices[3 * index.vertex_index];
            vertex.pos = { (-1.0) * (*(vp + 0)), *(vp + 1), *(vp + 2) * (-1) };

            /*if (!attrib.normals.empty() && index.normal_index >= 0)
            {
               const float* np = &attrib.normals[3 * index.normal_index];
               vertex.nrm = { *(np + 0), *(np + 1), *(np + 2) };
            }*/

            if (!attrib.texcoords.empty() && index.texcoord_index >= 0)
            {
                const float* tp = &attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.texCoord = { *tp, 1.0f - *(tp + 1) };
            }

            if (!attrib.colors.empty())
            {
                const float* vc = &attrib.colors[3 * index.vertex_index];
                vertex.color = { *(vc + 0), *(vc + 1), *(vc + 2) };
            }

            m_vertices.push_back(vertex);
            m_indices.push_back(static_cast<int>(m_indices.size()));
        }
    }

    m_objmesh.nVertices = m_vertices.size();
    m_objmesh.nIndices = m_indices.size();
    m_objmesh.vertices = m_vertices;
    m_objmesh.indices = m_indices;
}

void ObjLoader::createBackground(glm::vec3 bgSize)
{
    clearCurrentInfo();

    float length = bgSize.x;
    float width = bgSize.y;

    float originx = -length / 2.0;
    float originy = -width / 2.0;
    float finalx = length / 2.0;
    float finaly = width / 2.0;

    nvmath::vec3f points[6];
    points[0].x = originx;
    points[1].x = originx;
    points[2].x = finalx;
    points[3].x = finalx;
    points[0].z = originy;
    points[1].z = finaly;
    points[2].z = finaly;
    points[3].z = originy;
    points[0].y = 0;
    points[1].y = 0;
    points[2].y = 0;
    points[3].y = 0;

    int list[] = {0,1,2,0,2,3};
    for (int i = 0; i < 6; i++)
    {
        int listi = list[i];
        VertexAttribute va{ points[listi] };
        m_objmesh.vertices.emplace_back(va);
    }

    m_objmesh.indices.emplace_back(0);
    m_objmesh.indices.emplace_back(0 + 1);
    m_objmesh.indices.emplace_back(0 + 2);
    m_objmesh.indices.emplace_back(0 + 3);
    m_objmesh.indices.emplace_back(0 + 4);
    m_objmesh.indices.emplace_back(0 + 5);

    m_objmesh.nIndices = m_objmesh.indices.size();
    m_objmesh.nVertices = m_objmesh.vertices.size();
    minElevation = 0;
}

void ObjLoader::creatBackgroundFromDEM(const std::string& filename, nvmath::vec3f sceneSize)
{
    clearCurrentInfo();

    CPLSetConfigOption("gdal_filename_is_utf8", "no");
    float scale = 1.0;

    std::string infile = filename;
    GDALAllRegister();

    float * pafScanblock;
    GDALDataset* poDataset;
    poDataset = (GDALDataset*)GDALOpen(infile.c_str(), GA_ReadOnly);
    if (poDataset == NULL)
    {
        std::cout << "fail in open files!" << std::endl;
        return;
    }
    int nImgSizeX = poDataset->GetRasterXSize();
    int nImgSizeY = poDataset->GetRasterYSize();
    int bandcount = poDataset->GetRasterCount();

    int num_image_size = 0;
    float * pafScanline = new float[nImgSizeX * nImgSizeY * bandcount];
    poDataset->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanline, nImgSizeX, nImgSizeY, GDT_Float32, bandcount, 0, 0, 0, 0);

    scale = sceneSize.x / nImgSizeX;

    cv::Mat A(nImgSizeX,nImgSizeY,CV_32F, pafScanline);

    int newX = nImgSizeX * scale;
    int newY = nImgSizeY * scale;
//    float * newImage = new float[newX*newY];
    cv::Mat newImage(newX,newY,CV_32F,0);
    cv::resize(A,newImage,cv::Size(newX,newY),cv::INTER_CUBIC);
//    std::vector<double> x, y, z;
//    for (int i = 0; i < nImgSizeX; i++)
//    {
//        for (int j = 0; j < nImgSizeY; j++)
//        {
//            x.emplace_back(i * scale + scale / 2.0);
//            y.emplace_back(j * scale + scale / 2.0);
//            z.emplace_back(pafScanline[i * nImgSizeY + j]);
//           /* y.emplace_back(i * scale + scale / 2.0);
//            x.emplace_back(j * scale + scale / 2.0);
//            z.emplace_back(pafScanline[i * nImgSizeY + j]);*/
//        }
//    }
//    _2D::ThinPlateSplineInterpolator<double> interp;
//    interp.setData(x, y, z);


    float step = 1/10.0;
    delete[] pafScanline;
    delete poDataset;
    scale = sceneSize.x / nImgSizeX;

    //scale = 1;
    //std::vector<nvmath::vec3f> points;
    uint32_t nVertices = 0;
    uint32_t nIndices = 0;
//    minElevation = newImage.at<float>(0,0);
    minElevation = *std::min_element(newImage.begin<float>(), newImage.end<float>());
    centerElevation = (*std::max_element(newImage.begin<float>(), newImage.end<float>()) + minElevation) / 2.0;
    for (float j = 0.0; j < nImgSizeX; j = j + step)
    {
        for (float k = 0.0; k < nImgSizeY; k = k + step)
        {
            float jj1, kk1, jj2, kk2;

            jj1 = j * scale;
            jj2 = (j + step) * scale;
            kk1 = k * scale;
            kk2 = (k + step) * scale;

            if (jj1 < scale / 2.0) jj1 = scale / 2.0;
            if (jj2 < scale / 2.0) jj2 = scale / 2.0;
            if (kk1 < scale / 2.0) kk1 = scale / 2.0;
            if (kk2 < scale / 2.0) kk2 = scale / 2.0;
            if (jj1 > nImgSizeX * scale - scale / 2.0) jj1 = nImgSizeX * scale - scale / 2.0 - 0.01;
            if (jj2 > nImgSizeX * scale - scale / 2.0) jj2 = nImgSizeX * scale - scale / 2.0 - 0.01;
            if (kk1 > nImgSizeY * scale - scale / 2.0) kk1 = nImgSizeY * scale - scale / 2.0 - 0.01;
            if (kk2 > nImgSizeY * scale - scale / 2.0) kk2 = nImgSizeY * scale - scale / 2.0 - 0.01;

            nvmath::vec3f point1 = { j * scale - sceneSize.x / 2.0, newImage.at<float>(jj1, kk1) - centerElevation, k * scale - sceneSize.y/2.0 };
            nvmath::vec3f point2 = { j * scale - sceneSize.x / 2.0, newImage.at<float>(jj1, kk2) - centerElevation, (k + step) * scale  - sceneSize.y/2.0};
            nvmath::vec3f point3 = { (j + step) * scale - sceneSize.x / 2.0, newImage.at<float>(jj2, kk2) - centerElevation, (k + step) * scale  - sceneSize.y/2.0};
            nvmath::vec3f point4 = { (j + step) * scale - sceneSize.x / 2.0, newImage.at<float>(jj2, kk1) - centerElevation, k * scale  - sceneSize.y/2.0};

//            if (minElevation > point1.y) minElevation = point1.y;
//            if (minElevation > point2.y) minElevation = point2.y;
//            if (minElevation > point3.y) minElevation = point3.y;
//            if (minElevation > point4.y) minElevation = point4.y;

            VertexAttribute va1{ point1 };
            m_vertices.emplace_back(va1);
            VertexAttribute va2{ point2 };
            m_vertices.emplace_back(va2);
            VertexAttribute va3{ point3 };
            m_vertices.emplace_back(va3);
            VertexAttribute va4{ point4 };
            m_vertices.emplace_back(va4);


            m_indices.emplace_back(nVertices);
            m_indices.emplace_back(nVertices + 1);
            m_indices.emplace_back(nVertices + 2);
            m_indices.emplace_back(nVertices + 0);
            m_indices.emplace_back(nVertices + 2);
            m_indices.emplace_back(nVertices + 3);
            nVertices = nVertices + 4;
            nIndices = nIndices + 2 * 3;
        }
    }

    m_objmesh.nVertices = nVertices;
    m_objmesh.nIndices = nIndices;
    m_objmesh.vertices = m_vertices;
    m_objmesh.indices = m_indices;

    m_heightmap = newImage.clone(); // 保存插值后的高程图
}

//void ObjLoader::createBackgroundFromDEM(const std::string& filename, nvmath::vec3f sceneSize_XYZ, _2D::ThinPlateSplineInterpolator<double>& interp)
//{
//    clearCurrentInfo();
//
//    CPLSetConfigOption("gdal_filename_is_utf8", "no");
//    float scale = 1.0;
//
//    std::string infile = filename;
//    GDALAllRegister();
//
//    BYTE* pafScanblock;
//    GDALDataset* poDataset;
//    poDataset = (GDALDataset*)GDALOpen(infile.c_str(), GA_ReadOnly);
//    if (poDataset == NULL)
//    {
//        std::cout << "fail in open files!" << std::endl;
//        return;
//    }
//    int nImgSizeX = poDataset->GetRasterXSize();
//    int nImgSizeY = poDataset->GetRasterYSize();
//    int bandcount = poDataset->GetRasterCount();
//
//    int num_image_size = 0;
//    /*BYTE* pafScanline = new BYTE[nImgSizeX * nImgSizeY * bandcount];
//    poDataset->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanline, nImgSizeX, nImgSizeY, GDT_Byte, bandcount, 0, 0, 0, 0);*/
//
//    FLOAT* pafScanline = new FLOAT[nImgSizeX * nImgSizeY];
//    poDataset->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanline, nImgSizeX, nImgSizeY, GDT_Float32, bandcount, 0, 0, 0, 0);
//
//    scale = sceneSize_XYZ.x / nImgSizeX;
//
//    std::vector<double> x, y, z;
//    for (int i = 0; i < nImgSizeX; i++)
//    {
//        for (int j = 0; j < nImgSizeY; j++)
//        {
//            x.emplace_back(i * scale + scale / 2.0);
//            y.emplace_back(j * scale + scale / 2.0);
//            z.emplace_back(pafScanline[i * nImgSizeY + j]);
//        }
//    }
//    interp.setData(x, y, z);
//
//    float step = 1.0 / 2.0;
//    delete[] pafScanline;
//    delete poDataset;
//
//
//    //scale = 1;
//    //std::vector<nvmath::vec3f> points;
//    uint32_t nVertices = 0;
//    uint32_t nIndices = 0;
//    minElevation = z[0];
//    for (float j = 0.0; j < nImgSizeX; j = j + step)
//    {
//        for (float k = 0.0; k < nImgSizeY; k = k + step)
//        {
//            float jj1, kk1, jj2, kk2;
//
//            jj1 = j * scale;
//            jj2 = (j + step) * scale;
//            kk1 = k * scale;
//            kk2 = (k + step) * scale;
//
//            if (jj1 < scale / 2.0) jj1 = scale / 2.0;
//            if (jj2 < scale / 2.0) jj2 = scale / 2.0;
//            if (kk1 < scale / 2.0) kk1 = scale / 2.0;
//            if (kk2 < scale / 2.0) kk2 = scale / 2.0;
//            if (jj1 > nImgSizeX * scale - scale / 2.0) jj1 = nImgSizeX * scale - scale / 2.0 - 0.01;
//            if (jj2 > nImgSizeX * scale - scale / 2.0) jj2 = nImgSizeX * scale - scale / 2.0 - 0.01;
//            if (kk1 > nImgSizeY * scale - scale / 2.0) kk1 = nImgSizeY * scale - scale / 2.0 - 0.01;
//            if (kk2 > nImgSizeY * scale - scale / 2.0) kk2 = nImgSizeY * scale - scale / 2.0 - 0.01;
//
//            nvmath::vec3f point1 = { j * scale, interp(jj1, kk1), k * scale };
//            nvmath::vec3f point2 = { j * scale, interp(jj1, kk2), (k + step) * scale };
//            nvmath::vec3f point3 = { (j + step) * scale, interp(jj2, kk2), (k + step) * scale };
//            nvmath::vec3f point4 = { (j + step) * scale, interp(jj2, kk1), k * scale };
//
//            if (minElevation > point1.y) minElevation = point1.y;
//            if (minElevation > point2.y) minElevation = point2.y;
//            if (minElevation > point3.y) minElevation = point3.y;
//            if (minElevation > point4.y) minElevation = point4.y;
//
//            VertexAttribute va1{ point1 };
//            m_vertices.emplace_back(va1);
//            VertexAttribute va2{ point2 };
//            m_vertices.emplace_back(va2);
//            VertexAttribute va3{ point3 };
//            m_vertices.emplace_back(va3);
//            VertexAttribute va4{ point4 };
//            m_vertices.emplace_back(va4);
//
//
//            m_indices.emplace_back(nVertices);
//            m_indices.emplace_back(nVertices + 1);
//            m_indices.emplace_back(nVertices + 2);
//            m_indices.emplace_back(nVertices + 0);
//            m_indices.emplace_back(nVertices + 2);
//            m_indices.emplace_back(nVertices + 3);
//            nVertices = nVertices + 4;
//            nIndices = nIndices + 2 * 3;
//        }
//    }
//
//    m_objmesh.nVertices = nVertices;
//    m_objmesh.nIndices = nIndices;
//    m_objmesh.vertices = m_vertices;
//    m_objmesh.indices = m_indices;
//}
//
//void ObjLoader:: createBackgroundFromDEM(const std::string& filename, nvmath::vec3f sceneSize_XYZ, _2D::BilinearInterpolator<double>& interp)
//{
//    clearCurrentInfo();
//
//    CPLSetConfigOption("gdal_filename_is_utf8", "no");
//
//    std::string infile = filename;
//    GDALAllRegister();
//
//    BYTE* pafScanblock;
//    GDALDataset* poDataset;
//    poDataset = (GDALDataset*)GDALOpen(infile.c_str(), GA_ReadOnly);
//    if (poDataset == NULL)
//    {
//        std::cout << "fail in open files!" << std::endl;
//        return;
//    }
//    nImgSizeX = poDataset->GetRasterXSize();
//    nImgSizeY = poDataset->GetRasterYSize();
//    int bandcount = poDataset->GetRasterCount();
//
//    int num_image_size = 0;
//
//    FLOAT* pafScanline = new FLOAT[nImgSizeX * nImgSizeY];
//    poDataset->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanline, nImgSizeX, nImgSizeY, GDT_Float32, bandcount, 0, 0, 0, 0);
//
//    m_scale = sceneSize_XYZ.x / nImgSizeX;
//    float scale = m_scale;
//
//    std::vector<double> x, y, z;
//    for (int i = 0; i < nImgSizeX; i++)
//    //for (int i = nImgSizeX-1; i >=0; i--)
//    {
//        for (int j = 0; j < nImgSizeY; j++)
//        {
//            //x.emplace_back(sceneSize_XYZ.x - (i * scale + scale / 2.0)); // pixel center location
//            //y.emplace_back(j * scale + scale / 2.0);
//            //z.emplace_back(pafScanline[i * nImgSizeY + j]);
//            x.emplace_back((i * scale + scale / 2.0)); // pixel center location
//            y.emplace_back((j * scale + scale / 2.0));
//            z.emplace_back(pafScanline[i * nImgSizeY + j]);
//        }
//    }
//
//    /*for (int i = 0; i < nImgSizeX; i++)
//    {
//        x[i] = sceneSize_XYZ.x - x[i];
//    }*/
//
//    interp.setData(x, y, z);
//
//    delete[] pafScanline;
//    delete poDataset;
//
//    float step = m_step;// / 2.0; // control DEM subdivision degree, The smaller the value, the finer the distinction
//
//    uint32_t nVertices = 0;
//    uint32_t nIndices = 0;
//    minElevation = z[0];
//    for(float j = 0.0; j < nImgSizeX; j = j + step)
//    {
//        for (float k = 0.0; k < nImgSizeY; k = k + step)
//        {
//            float jj1, kk1, jj2, kk2;
//
//            /*jj1 = j * scale;
//            jj2 = (j + step) * scale;*/
//
//            // 将 x 轴对换，与envi保持一致性
//            jj1 = sceneSize_XYZ.x - j * scale;
//            jj2 = sceneSize_XYZ.x - (j + step) * scale;
//            kk1 = k * scale;
//            kk2 = (k + step) * scale;
//
//            if (jj1 < scale / 2.0) jj1 = scale / 2.0;
//            if (jj2 < scale / 2.0) jj2 = scale / 2.0;
//            if (kk1 < scale / 2.0) kk1 = scale / 2.0;
//            if (kk2 < scale / 2.0) kk2 = scale / 2.0;
//            if (jj1 > nImgSizeX * scale - scale / 2.0) jj1 = nImgSizeX * scale - scale / 2.0 - 0.01;
//            if (jj2 > nImgSizeX * scale - scale / 2.0) jj2 = nImgSizeX * scale - scale / 2.0 - 0.01;
//            if (kk1 > nImgSizeY * scale - scale / 2.0) kk1 = nImgSizeY * scale - scale / 2.0 - 0.01;
//            if (kk2 > nImgSizeY * scale - scale / 2.0) kk2 = nImgSizeY * scale - scale / 2.0 - 0.01;
//
//            nvmath::vec3f point1 = { j * scale, interp(jj1, kk1), k * scale };
//            nvmath::vec3f point2 = { j * scale, interp(jj1, kk2), (k + step) * scale };
//            nvmath::vec3f point3 = { (j + step) * scale, interp(jj2, kk2), (k + step) * scale };
//            nvmath::vec3f point4 = { (j + step) * scale, interp(jj2, kk1), k * scale };
//
//            point1 = point1 - nvmath::vec3f({ sceneSize_XYZ.x / 2.0, 0, sceneSize_XYZ.y / 2.0 });
//            point2 = point2 - nvmath::vec3f({ sceneSize_XYZ.x / 2.0, 0, sceneSize_XYZ.y / 2.0 });
//            point3 = point3 - nvmath::vec3f({ sceneSize_XYZ.x / 2.0, 0, sceneSize_XYZ.y / 2.0 });
//            point4 = point4 - nvmath::vec3f({ sceneSize_XYZ.x / 2.0, 0, sceneSize_XYZ.y / 2.0 });
//
//            if (minElevation > point1.y) minElevation = point1.y;
//            if (minElevation > point2.y) minElevation = point2.y;
//            if (minElevation > point3.y) minElevation = point3.y;
//            if (minElevation > point4.y) minElevation = point4.y;
//
//            VertexAttribute va1{ point1 };
//            m_vertices.emplace_back(va1);
//            VertexAttribute va2{ point2 };
//            m_vertices.emplace_back(va2);
//            VertexAttribute va3{ point3 };
//            m_vertices.emplace_back(va3);
//            VertexAttribute va4{ point4 };
//            m_vertices.emplace_back(va4);
//
//
//            m_indices.emplace_back(nVertices);
//            m_indices.emplace_back(nVertices + 1);
//            m_indices.emplace_back(nVertices + 2);
//            m_indices.emplace_back(nVertices + 0);
//            m_indices.emplace_back(nVertices + 2);
//            m_indices.emplace_back(nVertices + 3);
//            nVertices = nVertices + 4;
//            nIndices = nIndices + 2 * 3;
//        }
//    }
//
//    m_objmesh.nVertices = nVertices;
//    m_objmesh.nIndices = nIndices;
//    m_objmesh.vertices = m_vertices;
//    m_objmesh.indices = m_indices;
//}
//
//void ObjLoader::createBackgroundFromResizedDEM(const std::string &filename, nvmath::vec3f sceneSize_XYZ, _2D::BilinearInterpolator<double> &interp)
//{
//    clearCurrentInfo();
//
//    CPLSetConfigOption("gdal_filename_is_utf8", "no");
//
//    std::string infile = filename;
//    GDALAllRegister();
//
//    BYTE *pafScanblock;
//    GDALDataset *poDataset;
//    poDataset = (GDALDataset *)GDALOpen(infile.c_str(), GA_ReadOnly);
//    if (poDataset == NULL)
//    {
//        std::cout << "fail in open files!" << std::endl;
//        return;
//    }
//    nImgSizeX = poDataset->GetRasterXSize();
//    nImgSizeY = poDataset->GetRasterYSize();
//    int bandcount = poDataset->GetRasterCount();
//
//    int num_image_size = 0;
//
//    FLOAT *pafScanline = new FLOAT[nImgSizeX * nImgSizeY];
//    poDataset->RasterIO(GF_Read, 0, 0, nImgSizeX, nImgSizeY, pafScanline, nImgSizeX, nImgSizeY, GDT_Float32, bandcount, 0, 0, 0, 0);
//
//    m_scale = sceneSize_XYZ.x / nImgSizeX;
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
//    interp.setData(x, y, z);
//
//    delete[] pafScanline;
//    delete poDataset;
//
//    float step = m_step; // / 2.0; // control DEM subdivision degree, The smaller the value, the finer the distinction
//    //step = 5;
//
//    uint32_t nVertices = 0;
//    uint32_t nIndices = 0;
//    minElevation = z[0];
//
//    for (float j = 0.0; j < sceneSize_XYZ.x; j = j + step)
//    {
//        for (float k = 0.0; k < sceneSize_XYZ.y; k = k + step)
//        {
//            float jj1, kk1, jj2, kk2;
//
//            // 将 x 轴对换，与envi保持一致性
//            jj1 = sceneSize_XYZ.x - j;
//            jj2 = sceneSize_XYZ.x - (j + step);
//            kk1 = k;
//            kk2 = (k + step);
//
//            /*if (jj1 < step / 2.0)
//                jj1 = step / 2.0;
//            if (jj2 < step / 2.0)
//                jj2 = step / 2.0;
//            if (kk1 < step / 2.0)
//                kk1 = step / 2.0;
//            if (kk2 < step / 2.0)
//                kk2 = step / 2.0;
//            if (jj1 > sceneSize_XYZ.x - step / 2.0)
//                jj1 = sceneSize_XYZ.x - step / 2.0 - 0.01;
//            if (jj2 > sceneSize_XYZ.x - step / 2.0)
//                jj2 = sceneSize_XYZ.x - step / 2.0 - 0.01;
//            if (kk1 > sceneSize_XYZ.y - step / 2.0)
//                kk1 = sceneSize_XYZ.y - step / 2.0 - 0.01;
//            if (kk2 > sceneSize_XYZ.y - step / 2.0)
//                kk2 = sceneSize_XYZ.y - step / 2.0 - 0.01;*/
//
//            float correctValue = 0.02;
//            if (jj1 < m_scale/2.0)
//                jj1 = m_scale / 2.0 + correctValue;
//            if (jj2 < m_scale / 2.0)
//                jj2 = m_scale / 2.0 + correctValue;
//            if (kk1 < m_scale / 2.0)
//                kk1 = m_scale / 2.0 + correctValue;
//            if (kk2 < m_scale / 2.0)
//                kk2 = m_scale / 2.0 + correctValue;
//            if (jj1 > sceneSize_XYZ.x - m_scale / 2.0)
//                jj1 = sceneSize_XYZ.x - m_scale / 2.0 - correctValue;
//            if (jj2 > sceneSize_XYZ.x - m_scale / 2.0)
//                jj2 = sceneSize_XYZ.x - m_scale / 2.0 - correctValue;
//            if (kk1 > sceneSize_XYZ.y - m_scale / 2.0)
//                kk1 = sceneSize_XYZ.y - m_scale / 2.0 - correctValue;
//            if (kk2 > sceneSize_XYZ.y - m_scale / 2.0)
//                kk2 = sceneSize_XYZ.y - m_scale / 2.0 - correctValue;
//
//            nvmath::vec3f point1 = {j, interp(jj1, kk1), k};
//            nvmath::vec3f point2 = {j, interp(jj1, kk2), (k + step)};
//            nvmath::vec3f point3 = {(j + step), interp(jj2, kk2), (k + step)};
//            nvmath::vec3f point4 = {(j + step), interp(jj2, kk1), k};
//
//            point1 = point1 - nvmath::vec3f({sceneSize_XYZ.x / 2.0, 0, sceneSize_XYZ.y / 2.0});
//            point2 = point2 - nvmath::vec3f({sceneSize_XYZ.x / 2.0, 0, sceneSize_XYZ.y / 2.0});
//            point3 = point3 - nvmath::vec3f({sceneSize_XYZ.x / 2.0, 0, sceneSize_XYZ.y / 2.0});
//            point4 = point4 - nvmath::vec3f({sceneSize_XYZ.x / 2.0, 0, sceneSize_XYZ.y / 2.0});
//
//            if (minElevation > point1.y)
//                minElevation = point1.y;
//            if (minElevation > point2.y)
//                minElevation = point2.y;
//            if (minElevation > point3.y)
//                minElevation = point3.y;
//            if (minElevation > point4.y)
//                minElevation = point4.y;
//
//            VertexAttribute va1{point1};
//            m_vertices.emplace_back(va1);
//            VertexAttribute va2{point2};
//            m_vertices.emplace_back(va2);
//            VertexAttribute va3{point3};
//            m_vertices.emplace_back(va3);
//            VertexAttribute va4{point4};
//            m_vertices.emplace_back(va4);
//
//            m_indices.emplace_back(nVertices);
//            m_indices.emplace_back(nVertices + 1);
//            m_indices.emplace_back(nVertices + 2);
//            m_indices.emplace_back(nVertices + 0);
//            m_indices.emplace_back(nVertices + 2);
//            m_indices.emplace_back(nVertices + 3);
//            nVertices = nVertices + 4;
//            nIndices = nIndices + 2 * 3;
//        }
//    }
//
//    m_objmesh.nVertices = nVertices;
//    m_objmesh.nIndices = nIndices;
//    m_objmesh.vertices = m_vertices;
//    m_objmesh.indices = m_indices;
//}
//
//double ObjLoader::getShiftInterp(_2D::BilinearInterpolator<double> interp, nvmath::vec3f shift0)
//{
//    float scale = m_scale;
//
//    shift0.x = nImgSizeX * scale - shift0.x;//d与envi对齐
//
//    float correct = 1;
//    if (shift0.x < scale / 2.0)
//        shift0.x = scale / 2.0 + 1;
//    if (shift0.y < scale / 2.0)
//        shift0.y = scale / 2.0 + 1;
//    if (shift0.x > nImgSizeX * scale - scale / 2.0)
//        shift0.x = nImgSizeX * scale - scale / 2.0 - 1;
//    if (shift0.y > nImgSizeX * scale - scale / 2.0)
//        shift0.y = nImgSizeX * scale - scale / 2.0 - 1;
//
//    //float correctValue = 1;
//    ///*if (shift0.x < m_step - nImgSizeX * scale/2)
//    //    shift0.x = m_step - nImgSizeX * scale / 2 + correctValue;
//    //if (shift0.y < m_step - nImgSizeY * scale / 2)
//    //    shift0.y = m_step - nImgSizeY * scale / 2 + correctValue;
//    //if (shift0.x > nImgSizeX * scale - nImgSizeX * scale / 2 - m_step)
//    //    shift0.x = nImgSizeX * scale - nImgSizeX * scale / 2 - m_step - correctValue;
//    //if (shift0.y > shift0.y - nImgSizeY * scale / 2 - m_scale)
//    //    shift0.y = nImgSizeX * scale - nImgSizeY * scale / 2 - m_step - correctValue;*/
//    //if (shift0.x < m_step)
//    //    shift0.x = m_step + correctValue;
//    //if (shift0.y < m_step)
//    //    shift0.y = m_step + correctValue;
//    //if (shift0.x > nImgSizeX * scale - m_step)
//    //    shift0.x = nImgSizeX * scale - m_step - correctValue;
//    //if (shift0.y > nImgSizeY * scale - m_step)
//    //    shift0.y = nImgSizeY * scale - m_step - correctValue;
//
//    ////shift0.x = nImgSizeX * scale - shift0.x; //d与envi对齐
//
//    return interp(shift0.x, shift0.y);
//}



void ObjLoader::clearCurrentInfo()
{
//    m_vertices.swap(std::vector<VertexAttribute>());
//    m_indices.swap(std::vector<uint32_t>());
}

float ObjLoader::getElevation(float locX, float locY) {
    float locZ = 0;

    return locZ;
}
void ObjLoader::interpolateZValues(nvmath::vec3f sceneSize, float* tempx, float* tempy,
    float* tempz, int num_points)
{
    if (m_heightmap.empty()) {
        std::cerr << "Error: Heightmap not initialized. Call creatBackgroundFromDEM first." << std::endl;
        return;
    }
    int width = m_heightmap.cols;
    int height = m_heightmap.rows;

    // 2. 检查坐标是否在场景范围内
    for (int i = 0; i < num_points; i++) {
        if (tempx[i] < 0 || tempx[i] > sceneSize.x || tempy[i] < 0 || tempy[i] > sceneSize.y) {
            std::cerr << "Warning: Coordinate (" << tempx[i] << ", " << tempy[i] << ") out of bounds!" << std::endl;
            tempz[i] = 0; // 默认填充 0 或抛出异常
            continue;
        }
    }

    // 3. 构建映射矩阵
    cv::Mat map_x(1, num_points, CV_32F);
    cv::Mat map_y(1, num_points, CV_32F);

    for (int i = 0; i < num_points; i++) {
        float v = (tempx[i] / sceneSize.x) * (width - 1);
        float u = (tempy[i] / sceneSize.y) * (height - 1);
        // map_x.at<float>(0, i) = u;
        // map_y.at<float>(0, i) = v;
        // map_x.at<float>(0, i) = v;
        // map_y.at<float>(0, i) = u;

        // 边界检查
        if (u < 0 || u >= width-1 || v < 0 || v >= height-1) {
            tempz[i] = 0.0f;
            continue;
        }

        // 双线性插值
        int x0 = floor(u);
        int y0 = floor(v);
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        float dx = u - x0;
        float dy = v - y0;

        float z00 = m_heightmap.at<float>(y0, x0);
        float z10 = m_heightmap.at<float>(y0, x1);
        float z01 = m_heightmap.at<float>(y1, x0);
        float z11 = m_heightmap.at<float>(y1, x1);

        float zInterp =
            (1 - dx) * (1 - dy) * z00 +
            dx       * (1 - dy) * z10 +
            (1 - dx) * dy       * z01 +
            dx       * dy       * z11;

        tempz[i] = zInterp;
    }


}





