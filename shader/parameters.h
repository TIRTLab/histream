#ifndef STRUCTURE
#define STRUCTURE 1

// #ifdef __cplusplus
// #include <stdint.h>
// #include "nvmath/nvmath.h"
// // GLSL Type
// using ivec2 = nvmath::vec2i;
// using vec2  = nvmath::vec2f;
// using vec3  = nvmath::vec3f;
// using vec4  = nvmath::vec4f;
// using mat4  = nvmath::mat4f;
// using uint  = unsigned int;
// #endif

#include "global.glsl"

//////////////////////////////////////////////////////////////
//   Parameters used in Input
//////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
// Entity /Component Property
//--------------------------------------------------------------------------------------------

// Wavelength link to sunlight source and camera metric as well as optical material


// reflectance and transmittance
struct SpectralMaterial
{
	float reflectance;
	float transmittance;
};
// sunlit and shaded temperature
struct ThermalMaterial
{
	float sunlitTemperature;
	float shadedTemperature;
};

// canopy information
struct Canopy
{
	float density;
	float hc;
	float G;
	float LIDFa;
	float LIDFb;
	float hspot;
	float leafwidth;
};

// sphere instance defination
struct Sphere
{
  vec3  center;
  float radius;
};

// aabb instance defination
struct AllAabb
{
  vec3 minimum;
  int empty;
  vec3 maximum;
};

struct hitValue
{
	float id;
};

//--------------------------------------------------------------------------------------------
// Entity / Component
//--------------------------------------------------------------------------------------------

struct VertexAttribute
{
	vec3 position;
	vec3 NRM;
	vec3 color;
	vec2 txt;
};

struct SensorMatrix
{
	mat4 viewInverse;
	mat4 projInverse;
	float focalDist;
	float aperture;
	int empty_;
	int empty;
	vec3 direction;
};

struct SensorWaveInd
{
	int value;
};

struct LightSet
{
	vec3  direction;
	float direct;
	float diffuse;
	float Rin;
	float Rli;
	float skyTemperature;
	float solarTemperature;
};

struct WaveSet
{
	float wavelength;
	float direct;
	float diffuse;
};

// obj model link
struct ModelLink
{
	int type;
	int spectralId;
	int thermalId;
	uint64_t vertexAddress;
	uint64_t indexAddress;
};

// obj instance link
struct InstanceLink
{
	int modelId;
};

// instance link in voxel tracing
struct VoxelInstanceLink
{
	int voxelIdOffset;

    int type;
    int spectralId;
    int thermalId;
    int canopyId;
	
	uint64_t vertexAddress;
	uint64_t indexAddress;
};

// voxel model link
struct VoxelModelLink
{
    int type;
    int spectralId;
    int thermalId;
    int canopyId;
};

// voxel link
struct VoxelLink
{
    vec3 voxelId;
    int instanceId;
};

// transmittance
struct VoxelDir
{
    float solar;
    float emitter;
};

// radiance
struct VoxelRad
{
    float cumulated;
    float induced;
};

#endif 