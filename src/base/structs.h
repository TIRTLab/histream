//
// Created by admin on 2024/1/24.
// It is used for the cpu part;
//

#ifndef FIELD_STRUCTS_H
#define FIELD_STRUCTS_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <map>

#include "nvvk/resourceallocator_vk.hpp"
#include "structs_cg.h"
#include "src/thirdparty/magic_enum.hpp"
#include "src/thirdparty/NanoVDB.h"
#include "src/thirdparty/nanoutil/GridBuilder.h"
#include "src/thirdparty/nanoutil/Primitives.h"

#define PI 3.1415926
#define VNIRMAXBAND 2001
#define TIRMAXBAND 2162
#define LSTMAXBAND 2162

#define GROUP_SIZEXY 8 // Same group size as in compute shader
#define GROUP_SIZEX 64
#define MAX_FRAMES_IN_FLIGHT 10
#define TLASTNUM 8
#define SENSOR_HEIGHT 3000
#define SENSOR_FOV    0.5
#define ANGLE_COR 0.1
#define DEG2RAD  0.017453292
#define DIFFUSENUM 32
#define N1 2001
#define N2 161

using BufferT = nanovdb::HostBuffer;

// all binding index in addBindings

enum class VoxelRadStage
{
    gap,
    directVNIR,
    directTIR,
    diffuseVNIR,
    diffuseTIR,
    out
};

enum class VoxellstStage
{
    gap,
    directVNIR,
    directTIR,
    diffuseVNIR,
    diffuseTIR,
    bio,
    aero,
    evapo,
    budget,
    updateL,
    updateTp,
    out
};

enum class VoxelTracingStage
{
    gap,
    run,
    outImage
};


enum class EBStage
{
    budget,
    updateL
};

enum class ETStage
{
    evapo,
    updateTp
};

enum Queues
{
    eGCT,
    eCompute,
    eTransfer
};


enum class Mode {
    eRaytracing,
    eVoxelLST,
};

enum RaytracingStageIndices
{
    eRaygen,
    eMiss,
    eMiss2,
    eClosestHit,
    eShaderGroupCount
};

enum class Projection
{
    PARALLAL,
    PERSPECTIVE
};

enum class spectralType
{
    CUSTOM,
    LEAFBIO,
    SOILSET,
    BUILDUP,
    OTHER
};

enum class Type
{
    NO,
    SOIL,
    VEGETATION,
    BUILDING
};

enum class ShapeType
{
    CUBE,
    ELLIPSOID,
    PLANE
};

struct FluspectParam
{
    float Cab;
    float Cw;
    float Cdm;
    float Cs;
    float N;
//    float refl_tir;
//    float trans_tir;
};

struct BSMParam
{
    float SMC;
    float BSMBrightness;
    float BSMlat;
    float BSMlon;
//    float refl_tir;
//    float trans_tir;
};

struct SpectralXml
{
    std::string spectralName;
    spectralType type;
//    int n_band;
    std::vector<float> reflectances;
    std::vector<float> transmittance;
    FluspectParam fp;
    BSMParam bsm;
    std::string path;
    float tau_tir;
    float refl_tir;
};

struct ThermalXml
{
    std::string thermalName;
    float sunlitTemperature;
    float shadedTemperature;
};

struct CanopyXml
{
    std::string canopyName;
    Canopy canopy;
};

//struct LeafBioXml{
//    LeafBio leafbio;
//    std::string leafbioname;
//};
//
//struct SoilSetXml{
//    SoilSet soilset;
//    std::string soilsetname;
//};


struct SensorXml
{
    std::string name;
    Projection projection;
    glm::vec2 resolution;			   // { WIDTH, HEIGHT }
    //std::vector<int> wavelengthInds;	   // int -> index; LST:6/1000; ray:0-num
    std::vector<glm::vec2> viewAngles; // { zenith, azimuth }
    std::vector<float> waves;
    bool isImage{true};
    bool isAlbedo{false};
    bool isTemperature{true};
    bool isDisplay{false};
};



struct LightXml {
    std::string name;
    glm::vec2 solarAngle;
    float direct;
    float diffuse;
    float skyTemperature{250};
    float solarTemperature{6000};
 //   float directdiffuseratio{1.0};
};

struct AtomosphereXml
{
    std::string esunFileName;
    std::string eskyFileName;
};

struct SettingXml
{
  //  bool isSaveImg{true}; // 0: not save img but statistical result; 1: save img and statistical result
  //  bool isAlbedo{false}; // 0: only work for angle set; 1: additional, get albedo result
   // bool isDEM{false};	  // use DEM file or not
  // 		  // i: the ith GPU work, but not work currently
  //  std::string outDir;	  // the outDir to read input and save imagal and statistical results
  //  bool isDisplay{0};
  //  bool isTemperature{0};

    int theGPU{0};
    int n_sample{32};
    int maxDepth{5};
//    bool isTemperature{false};
//    bool isDisplay{false};
//    bool isImage{false};
};

struct AeroCond
{
    float hc_veg;
    float hc_build;
    float lai;
    float cover;
};

enum AeroType
{
    defined,
    scaneCal,  // one aerocond for the whole scene
    gridCal,   // different aerocond for each grid
};

struct AeroCondXml
{
    AeroType aerotype;
    std::string name;
    AeroCond aerocond;

};

struct Background
{
    std::string spectralName;
    std::string thermalName;
    std::string AeroCond;
    //spectralType bgSpectralType;
    std::string bgPropName;
  //  std::string bgAeroName;
};


struct ObjEntity
{
    std::string objName;
    std::string filePath;
    std::vector<std::string> meshNames;
    std::vector<std::string> spectralNames;
    std::vector<std::string> thermalNames;

//    std::vector<std::string> canopyNames;
//    std::map<std::string, std::string> spectralNames; // { meshName: attributes }
//    std::map<std::string, std::string> thermalNames;  // { meshName: attributes }
    bool isLarge{false};
    std::vector<glm::vec3> objDistributions;
    std::vector<float> scales;
    std::vector<float> rotations;
};

struct Shape
{
    float height;
    float width;
    float length;
    glm::vec3 pos;
};

struct PrimEntity
{
    /// the vector size is always 1.
    std::string primitiveName;
    std::vector<std::string> meshNames;  // delete
    std::vector<std::string> spectralNames;
    std::vector<std::string> thermalNames;
    std::vector<std::string> canopyNames;
    std::vector<std::string> propNames;
//    std::vector<std::string> aeroNames;
   // std::vector<std::string> shapeName;
    std::vector<Type>      types;
    std::vector<ShapeType> shapetypes;        // for different meshes
    std::vector<Shape>     shapes;
   // std::vector<glm::vec3> meshDistributions; // for different meshes in the primitives
    std::vector<glm::vec3> primDistributions;
    std::vector<float> scales;
    std::vector<float> rotations;

};


//struct Material{
//
//    int n_band;
//    std::vector<ThermalXML> thermals;
//    std::vector<SpectralXML> spectrals;
//    std::vector<CanopyXML> canopies;
//};

struct Angle{
    float sza;
    float saa;
    float vza;
    float vaa;
};

struct SceneXml
{

    // scene information
    glm::vec3 sceneSize;       //
    glm::vec3 sceneOrigin;     // default : (0,0,0)
    glm::vec3 sMin{ -5,0,-5 };
    glm::vec3 sMax{ 5,5,5 };
    float stepsize_surface;
    float stepsize_atmosphere;

    //float stepSize;		       // DEM Sampling
    Background background;


    // Scene Component Information
    std::vector<ObjEntity> objEntities;
    std::vector<PrimEntity> primEntities;


    // for dem option
    bool isDEM;
    std::string DEMPath;
    glm::vec2 demResolution;   // to DEM resolution if(-1) change nothing;
};


struct ObjMesh
{
    int meshId;
    uint32_t nIndices;
    uint32_t nVertices;
    std::vector<VertexAttribute> vertices;
    std::vector<uint32_t>        indices;
};

struct PrimMesh
{
    int meshId;
    uint32_t nIndices;
    uint32_t nVertices;
    std::vector<VertexAttribute> vertices;
    std::vector<uint32_t>        indices;
    std::vector<glm::vec3> centers;
    glm::vec3 meshcenter;
};

struct RaytracingXml
{
    std::string projectDir;
    std::string shaderDir;
    // system setting
    SettingXml settingxml;

    // run setting
    LightXml lightxml;  // for solar angle
    SensorXml sensorxml; // for viewing angle
//    std::vector<WaveSet> wavesets;  // for viewing band
//

    // Scene structural, i.e., background and its result;
    SceneXml scenexml;

    // component materials
    std::vector<SpectralXml> spectralxmls;
    std::vector<ThermalXml> thermalxmls;

};

struct PropertyXml{
    std::string name;
    Type type;
    LeafBio leafbio;
    SoilSet soilset;
};


struct  VoxelLstXml
{
    // main IO part
    std::string projectDir;
    std::string definedDir;
    // system setting
    SettingXml settingxml;

    // run setting
    LightXml lightxml;  // for solar angle
    SensorXml sensorxml; // for viewing angle
    //std::vector<WaveSet> wavesets;  // for viewing band
    //std::vector<Wave> waves;

    // Scene structural, i.e., background and its result;
    SceneXml scenexml;

    // component materials
    std::vector<SpectralXml> spectralxmls;
    std::vector<ThermalXml> thermalxmls;
    std::vector<PropertyXml> propxmls;
    std::vector<CanopyXml> canopyxmls;
    std::vector<AeroCondXml> aerocondxmls;

    std::string meteopath;
    std::string skypath;
    std::string sunpath;

    // std::vector<LeafBioXml> leafbioxmls;
    // std::vector<SoilSetXml> soilsetxmls;
  //  std::vector<CanopyXml> canopyxmls;
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
    float Tsold;	//
    float SatWater;
    float dTime;
};

struct MeteoXml
{
    float z;
    float Tsold;
    float satWater;
    float dTime;
    float Lat;
    float Lon;
    std::string fileName;
    std::vector<Meteo> meteos;
};

struct Aabb
{
    glm::vec3 minimum;
    glm::vec3 maximum;
};
struct VoxelModel  // <-- Cube, Ellipsoid
{
    int modelId;
    std::vector<Aabb> aabbs;
    std::vector<glm::ivec3> centerPoints;
};

struct VoxelTriModel
{
    int modelId;
    uint32_t nIndices{0};
    uint32_t nVertices{0};
    std::vector<VertexAttribute> vertices;
    std::vector<uint32_t> indices;

    std::vector<glm::ivec3> centerPoints;
    glm::vec3 modelCenterPoint;
};





#endif //FIELD_STRUCTS_H