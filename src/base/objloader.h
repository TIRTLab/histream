/******************************************************************************
 * Copyright 1998-2018 NVIDIA Corp. All Rights Reserved.
 *****************************************************************************/

#pragma once
#ifdef USE_NEW_NVPRO_CORE
//#include "libInterpolate/Interpolate.hpp"
#else
//#include "Interpolate.hpp"
#endif
#include "thirdparty/tiny_obj_loader.h"
#include "structs.h"
#include "nvmath/nvmath.h"
#include <array>
#include <iostream>
#include <unordered_map>
#include <vector>
// #include "libInterpolate/Interpolate.hpp"
#include "gdal.h"
#include "gdal_priv.h"


struct ShapeInfo
{
	uint32_t offset;
	uint32_t nbFacet;
	std::string name;
	int matIndex;
};

class ObjLoader
{
public:
	void loadModel(const std::string& filename);
	void loadMesh(const std::string& filename, const std::string& meshname);
	void createBackground(glm::vec3);
	void creatBackgroundFromDEM(const std::string& filename, nvmath::vec3f sceneSize);
//
//	void createBackgroundFromDEM(const std::string& filename, nvmath::vec3f sceneSize_XYZ, _2D::ThinPlateSplineInterpolator<double>& interp);
//	void createBackgroundFromDEM(const std::string& filename, nvmath::vec3f sceneSize_XYZ, _2D::BilinearInterpolator<double>& interp);//�������ǰ���ԭ����DEM���ɣ�����������ʱ�볡����С��һ�£�
//    void createBackgroundFromResizedDEM(const std::string &filename, nvmath::vec3f sceneSize_XYZ, _2D::BilinearInterpolator<double> &interp);//�Ƚ�DEM��ֵΪ������С��������
//	double getShiftInterp(_2D::BilinearInterpolator<double> interp, nvmath::vec3f shift0);

	ObjMesh m_objmesh;

	float minElevation{ 0 };

private:
	std::vector<VertexAttribute> m_vertices;
	std::vector<uint32_t> m_indices;
    void clearCurrentInfo();

	std::vector<ShapeInfo> m_shapeInfo;

	float m_scale{1};
    float m_step{5};
    int nImgSizeX{1};
    int nImgSizeY{1};


};