//
// Created by admin on 2024/1/25.
// For the kernel struct used both in the cpu and gpu part;
//

#ifndef FIELD_STRUCTS_CG_H
#define FIELD_STRUCTS_CG_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <map>

#include "nvvk/resourceallocator_vk.hpp"




struct alignas(16) SensorMatrix
{
    glm::mat4 viewInverse;
    glm::mat4 projInverse;
    float focalDist;
    float aperture;
    int empty{ 0 };
    int empty_{ 0 };

    glm::vec3 direction;
};

struct alignas(16) LightSet
{
    glm::vec3 direction{ 100, 100, 0 };
    float direct{ 0.9 };
    float diffuse{ 0.1 };
    float Rin{600};
    float Rli{450};
    float skyTemperature{ 150.1 };
    float solarTemperature{ 6000.0 };

};

struct  Spectral{
    float reflectance;
    float transmittance;
};

struct Thermal{
    float sunlitTemperature;
    float shadedTemperature;
};

struct alignas(16) RayRTSetting
{
    int frame{ 0 };
    int maxDepth{ 20 };
    int n_sample{ 32 };//64
    int n_wave{ 3 };

    glm::ivec2 resolution{ 1000, 1000 }; // ->Sensor.resolution
    //nvmath::vec2f sceneSize{ 100.0, 100.0 };
    int band1{ 1 };
    int band2{ 2 };
    int band3{ 3 };
    int isDisplay{ 0 };
    int isTemperature{ 0 };
    float fireflyClampThreshold{ 0 };
    int debugging_mode{ 0 };
};


struct alignas(32)  VoxelLstSetting
{

    int frame{0};
    int maxIteration{3};
    int maxDepth{16};
    int maxStep{25};
    int n_sample{16};
    int n_wave{5};
    int n_jump{10};
    float scale{1};
    glm::ivec2 imageSize{100, 100};
    int isDisplay{0};
    int empty{80};
    glm::ivec3 voxelSize{50, 0, 50};
    int dumpy{0};
    int dumpyy{0};
};


// whole
struct WaveSet
{
    float wavelength{0};
    float direct{0};
    float diffuse{0};
};

struct Wave
{
    float wavelength;
};

struct Canopy
{
    float lai;
    float density;
    float height;
    float width;
    float G;
    float LIDFa;
    float LIDFb;
    float hspot; // for the hotspot
    float leafwidth; // for the aerodynamic boundary
};


struct InstanceLink
{
    uint32_t meshId;
};

struct  Instance
{
    int meshId;
    glm::mat4 object2worldMatrix;
    glm::mat4 world2objectMatrix;
};

struct MeshBuffer
{
    uint32_t nbIndices{ 0 };
    uint32_t nbVertices{ 0 };
    nvvk::Buffer   vertexBuffer;
    nvvk::Buffer   indexBuffer;
};

struct MeshLink
{
    int type;
    int spectralId;
    int thermalId;
    int canopyId;
    // int leafbioId;
    // int soilsetId;
    int bioId;
    uint64_t vertexAddress;
    uint64_t indexAddress;
};

// voxel link
struct VoxelLink
{
    glm::ivec3 voxelPos{0,0,0};
    int instanceId{0};
   // int meshId{0};
    int aeroId{0};
    int primId{0};
    int empty{0};
    int empty_{0};// which faces ? 0 center 1, up, 2 bottom, 3 left, 4 right, 5, forward, 6 backward
    // now only 0 and 1 was used for the veg and soil, respectively;
};


//namespace VOXELLST {
//// voxel model link
//    struct MeshLink {
//        int type;
//        int spectralId;
//        int thermalId;
//        int canopyId;
//        int leafbioId;
//        int soilsetId;
//        uint64_t vertexAddress;
//        uint64_t indexAddress;
//    };
//
//    struct InstanceLink{
//        int voxelIdOffset;
//        int type;
//        int spectralId;
//        int thermalId;
//        int canopyId;
//        int bioId;
//        int aeroId;
//    };
//}

struct VertexAttribute
{
    glm::vec3 pos;
    glm::vec3 nrm;
    glm::vec3 color;
    glm::vec2 texCoord;
};


struct VoxelDir
{
    float solar;
    float emitter;
};

struct VoxelRad
{
    float cumulated;
    float induced;
    //float diffuse;
};

struct VoxelTempe
{
    float sunlit;
    float shaded;
};

struct VoxelNetRad
{
    float directVrad;   // direct radiance at VNIR bands
    float diffuseVrad;  // diffuse radiance at VNIR bands
    float directTrad;   // direct radiance at TIR bands
    float diffuseTrad;  // diffuse radiance at TIR bands
};

struct VoxelPnet
{
    float directPnet;
    float diffusePnet;
};

// aerodynamic resistance
struct VoxelRaa
{
    float raa;  // aerodynamic resistance
};

struct AeroCond
{
    float L;
    float ustar;
    float hc_veg;
    float hc_build;
    float lai;
    float cover;
};


// surface resistance
struct VoxelRss
{
    float sunlit;   // surface resistance at sunlit surface in this voxel
    float shaded;   // surface resistance at shaded surface in this voxel
};

// Air density
struct VoxelAir
{
    float cs;       // Carbon surface
    float ci;       // Carbon inside
    float es;       // Water Surface
};

// Rn - H - LE - G = 0
struct VoxelHeatflux
{
    float Hsunlit;  // sensible heat flux of sunlit component
    float Hshaded;  // sensible heat flux of shaded component
    float LEsunlit; // latent heat flux of sunlit component
    float LEshaded; // latent heat flux of shaded component
    float Gsunlit;  // change in heat storage of sunlit component
    float Gshaded;  // change in heat storage of shaded component
};

// temperature at LAST time nodes
struct TLAST
{
    float sunlit;   // sunlit temperature
    float shaded;   // shaded temperature
};

struct EBState
{
    uint32_t count;
};






struct LeafBio
{
    float Vcmax;		// maximum carboxylation capacity (at optimum temperature)
    float m;			// ball-berry stomatal conductance parameter
    float BallBerry;
    float Type;
    float kV;			// extinction coefficient for a vertical profile
    float Rdparam;		// parameter for dark respiration
    float Tparam[5];
    float Tyear;
    float beta;
    float kNPQs;		// rate constant of sustained thermal dissipation
    float qLs;			// fraction of functional reaction centers
    float stressfactor;
    int Tcor;

    //FluspectParam fp;
};

struct SoilSet
{
    int method;
    float rss;			// soil resistance for evaporation
    float cs;			// volumetric heat capacity of the soil
    float rhos;
    float lambdas;
    float SMC;			// volumetric soil moisture content 0.25
//    float csSoil;
  //  float rbs;      // boundary prop
    float Tsoil;  // aeverage temperature 25
    float SatWater;

    //BSMParam bsm;
};

struct Meteo
{
    float z;
    float t;		// time node
    float u;		// wind speed
    float p;		// air pressure
    float Rin;		// incoming shortwave radiation
    float Rli;		// incoming longwave radiation
    float Ta;		// air temperature
    float sm;		// volumetric soil moisture content
    float ea;		// atmospheric vapor pressure
    float Ca;		// CO2 concentration in the air
    float Oa;		// O2 concentration in the air
   // float Tsold;	//
   // float SatWater;
    float dTime;
};

//struct alignas(16) VoxelRTSetting
//{
//    glm::vec2 size{100, 100};
//    int frame{0};
//    int maxIteration{1};
//    int maxSample{5};
//    int maxBand{5};
//    int maxStep{50};
//    float scale{1};
//    glm::vec3 voxelSize{100, 10, 100};
//    int band1{0};
//    int band2{0};
//    int band3{0};
//    int isDisplay{0};
//    int isTemperature{0};
//};


//struct alignas(16) AeroSetting
//{
//    glm::vec2 size;
//    float scale;
//    float empty;
//    glm::vec3 voxelSize;
//};
//
//struct alignas(16) BioSetting
//{
//    glm::vec3 voxelSize;
//    float scale;
//};
//
//struct alignas(16) ETSetting
//{
//    glm::vec2 size;
//    float scale;
//    float empty;
//    glm::vec3 voxelSize;
//};
//
//struct alignas(16) EBSetting
//{
//    int maxIteration{50};
//    float minTempe{250};
//    float maxTempe{350};
//    float wc{0.2};
//    int theIteration{3};
//    float scale{1.0};
//    float empty;
//    float empty1;
//    glm::vec3 voxelSize{10, 10, 10};
//};

// surface L
//struct alignas(16) SurfL
//{
//    float L;
//    float ustar; //  wind speed  / friction velocity
//    float hvmax;
//    float laimax;
//};

//struct AeroCoeff
//{
//    float zo;
//    float d;
//    float Cd;		// drag coefficient for the vegetation
//    float rbc;		//
//    float CR;		// drag coefficient for a isolated tree
//    float CD1;		// fitting parameter
//    float Psicor;	// roughness layer correction
//    float CSSOIL;	// drag coefficient for soil
//    float rbs;		// for boundary soil resistance
//    float rwc;		// for Aerodynamic resistance Within Canopy
//   // float rbc;
//};

#endif //FIELD_STRUCTS_CG_H
