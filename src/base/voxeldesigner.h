#pragma once

#include <vulkan/vulkan.hpp>
#include <span>
#include "nvvk/context_vk.hpp"
#include "nvvk/commands_vk.hpp"
#include "nvvk/descriptorsets_vk.hpp"
#include "nvvk/debug_util_vk.hpp"
#include "nvvk/shaders_vk.hpp"
#include "nvvk/buffers_vk.hpp"

#include "nvvk/resourceallocator_vk.hpp"
#include  "nvvk/pipeline_vk.hpp"
#include "nvvk/shadermodulemanager_vk.hpp"
//#include "objloader.h"
#include "src/base/queue.h"
#include <nvmath/nvmath.h>
#include "src/base/structs.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "src/base/utils.h"
#include <cmath>

// �������ɵ�λ�þ�Ϊxyz����ϵ�µ�����



class VoxelDesigner
{
public:
    void loadModel(ShapeType shapeType, Shape shape, float stepSize);
    
	VoxelModel m_voxelModel;

	std::vector<glm::ivec3> createBackground(float sceneLength, float sceneWidth, float stepSize);

	PrimMesh createTriBackground(float sceneLength, float sceneWidth, float stepSize);
    PrimMesh createTriEntity(ShapeType shapeType, Shape shape, float stepSize);
//    VoxelTriModel createTriEntity_rotate(ShapeType shapeType, Shape shape,
//                                         float stepSize, float rotateAngle,std::vector<glm::ivec3> centers);

	//VoxelTriModel createTriBackgroundFromDEM(const std::string &filename, nvmath::vec3f sceneSize,
 //                                            _2D::BilinearInterpolator<double> &interp);


    float minElevation{0};

private:
	// output: centerPoints
    std::vector<glm::ivec3> createEllipsoid(float height, float width, float stepSize);

	std::vector<glm::ivec3> createCube(float length, float width, float height, float stepSize);
	
	PrimMesh createTriCube(Shape shape, float stepSize);
    PrimMesh createTriEllipsoid(Shape shape, float stepSize);

	bool isInPloy(nvmath::vec2i testPoint, std::vector<nvmath::vec2f> ploys);


//	Mesh createTriCube_rotate(Shape shape, float stepSize,
//                                       float rotateAngle,,std::vector<glm::ivec3> centers);
	/*VoxelPrimitive createCubePrimitive(Cube cube);

	VoxelPrimitive createBackgroundPrimitive(Background background);

	VoxelPrimitive createEllipsoidPrimitive(Ellipsoid ellipsoid);*/
    //std::vector<nvmath::vec3i> m_centerPoints;

    float m_scale;

	struct Ellipsoid 
	{
        std::vector<nvmath::vec3i> minPos;
        int activeNum{0};
    };
};