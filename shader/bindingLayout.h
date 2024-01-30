#ifndef BINDINGLAYOUT
#define BINDINGLAYOUT

#include "settings.h"
#include "parameters.h"


#ifdef RAYTRACING
#define B_SPECTRAL 0
#define B_THERMAL 1
#define B_ModelLINK 2
#define B_INSTANCELINK 3
#define B_TLAS 4
#define B_SENSOR 5
#define B_LIGHT 6
#define B_WAVE 7
#define B_STORAGE 8

layout(binding = B_SPECTRAL) buffer _bufferOPTICAL { SpectralMaterial spectralMaterials[]; }; // --
layout(binding = B_THERMAL) buffer _bufferTHERMAL { ThermalMaterial thermalMaterials[]; };    // --
layout(binding = B_ModelLINK) buffer _bufferMODELLINK { ModelLink modelLinks[]; }; 
layout(binding = B_INSTANCELINK) buffer _BufferInstanceLink {InstanceLink instanceLinks[];};      // --
layout(binding = B_TLAS) uniform accelerationStructureEXT topLevelAS;
layout(binding = B_SENSOR) uniform _SensorMatrix { SensorMatrix sensorMatrix; };              // ??
// layout(binding = B_WAVEIND) buffer _bufferWaveInds {SensorWaveInd sensorWaveInds[];};
layout(binding = B_LIGHT) uniform _LightSet { LightSet lightSet; };                          // -- 
layout(binding = B_WAVE) buffer _bufferWAVE { float waveSets[]; };                         // --
layout(binding = B_STORAGE) buffer _bufferstorage { float outImage[]; };
layout(buffer_reference, scalar) buffer Vertices { VertexAttribute v[]; };
layout(buffer_reference, scalar) buffer Indices { uvec3 i[]; };
layout(push_constant) uniform _RtxState
{
  RayRTSetting setting;
};

#endif



#ifdef VOXELTRACING

#define B_SPECTRAL 0
#define B_THERMAL 1
#define B_CANOPY 2
#define B_INSTANCELINK 3
#define B_VOXELLINK 4
#define B_NANO 5
#define B_AABB 6
#define B_TLAS 7
#define B_SENSOR 8
#define B_WAVEIND 9
#define B_LIGHT 10
#define B_WAVE 11
#define B_VOXELDIR 12  
#define B_VOXELRAD 13    
#define B_STORE 14

layout(binding = B_SPECTRAL) buffer _bufferOPTICAL { SpectralMaterial spectralMaterials[]; };
layout(binding = B_THERMAL) buffer _bufferTHERMAL { ThermalMaterial thermalMaterials[]; };
layout(binding = B_CANOPY) buffer _BufferCanopy { Canopy canopies[]; };
layout(binding = B_INSTANCELINK) buffer _BufferInstancesLink { VoxelInstanceLink instanceLinks[]; };
layout(binding = B_VOXELLINK) buffer _BufferVoxelLink {VoxelLink voxelLinks[];};
layout(binding = B_NANO) buffer _BufferVoxelNano {uint pnanovdb_buf_data[];};
layout(binding = B_AABB) buffer _BufferAabb {AllAabb aabbs[];};
layout( binding = B_TLAS)		uniform accelerationStructureEXT topLevelAS;
layout( binding = B_SENSOR)uniform _SensorMatrix { SensorMatrix sensorMatrix; };
layout(binding = B_WAVEIND) buffer _bufferWaveInds {SensorWaveInd sensorWaveInds[];};
layout( binding = B_LIGHT) uniform _LightSet { LightSet lightSet; };
layout( binding = B_WAVE) buffer _bufferWAVE { WaveSet waveSets[]; };
layout( binding = B_VOXELDIR) buffer _bufferdir { VoxelDir voxelDirs[]; };
layout( binding = B_VOXELRAD) buffer _bufferrad { VoxelRad voxelRads[]; };
layout( binding = B_STORE) buffer _bufferstorage { float outImage[]; };
layout(buffer_reference, scalar) buffer Vertices { VertexAttribute v[]; };
layout(buffer_reference, scalar) buffer Indices { uvec3 i[]; };
layout(push_constant) uniform _RtxState
{
  VoxelRTSetting setting;
};

#endif

#ifdef VOXELTRITRACING

#define B_SPECTRAL 0
#define B_THERMAL 1
#define B_CANOPY 2
#define B_INSTANCELINK 3
#define B_VOXELLINK 4
#define B_NANO 5
#define B_TLAS 6
#define B_SENSOR 7
#define B_WAVEIND 8
#define B_LIGHT 9
#define B_WAVE 10
#define B_VOXELDIR 11  
#define B_VOXELRAD 12    
#define B_STORE 13

layout(binding = B_SPECTRAL) buffer _bufferOPTICAL { SpectralMaterial spectralMaterials[]; };
layout(binding = B_THERMAL) buffer _bufferTHERMAL { ThermalMaterial thermalMaterials[]; };
layout(binding = B_CANOPY) buffer _BufferCanopy { Canopy canopies[]; };
layout(binding = B_INSTANCELINK) buffer _BufferInstancesLink { VoxelInstanceLink instanceLinks[]; };
layout(binding = B_VOXELLINK) buffer _BufferVoxelLink {VoxelLink voxelLinks[];};
layout(binding = B_NANO) buffer _BufferVoxelNano {uint pnanovdb_buf_data[];};
layout( binding = B_TLAS)		uniform accelerationStructureEXT topLevelAS;
layout( binding = B_SENSOR)uniform _SensorMatrix { SensorMatrix sensorMatrix; };
layout(binding = B_WAVEIND) buffer _bufferWaveInds {SensorWaveInd sensorWaveInds[];};
layout( binding = B_LIGHT) uniform _LightSet { LightSet lightSet; };
layout( binding = B_WAVE) buffer _bufferWAVE { WaveSet waveSets[]; };
layout( binding = B_VOXELDIR) buffer _bufferdir { VoxelDir voxelDirs[]; };
layout( binding = B_VOXELRAD) buffer _bufferrad { VoxelRad voxelRads[]; };
layout( binding = B_STORE) buffer _bufferstorage { float outImage[]; };
layout(buffer_reference, scalar) buffer Vertices { VertexAttribute v[]; };
layout(buffer_reference, scalar) buffer Indices { uvec3 i[]; };
layout(push_constant) uniform _RtxState
{
  VoxelRTSetting setting;
};

#endif

#endif