#ifndef GLOBALS_GLSL
#define GLOBALS_GLSL 1

// global constants
#define POROSITY 1 //土壤的孔隙度因子
#define SSA 0.25 // single scattering albedo
#define GST 0.35
#define DENSITY 1

#define PI 3.14159265358979323
#define TWO_PI 6.28318530717958648
#define INFINITY 1e32
#define RngStateType uint // Random type
#define SHORTDISTANCE 0.01

/// type information
#define TYPE_NULL 0
#define TYPE_SOIL 1
#define TYPE_VEGETATION 2
#define TYPE_BUILDING 3
#define AEROTYPE_VEGETATION 0
#define AEROTYPE_URBAN 1

/// scene object type
#define KIND_CUBE 0

//precision highp int;
precision highp float;

const float M_PI = 3.14159265358979323846;   // pi
const float M_TWO_PI = 6.28318530717958648;      // 2*pi
const float M_PI_2 = 1.57079632679489661923;   // pi/2
const float M_PI_4 = 0.785398163397448309616;  // pi/4
const float M_1_OVER_PI = 0.318309886183790671538;  // 1/pi
const float M_2_OVER_PI = 0.636619772367581343076;  // 2/pi
const float OUTER = 10000.0;
const float InvPI = 0.318309886183790671538;
#define EPSION_1 0.1
#define EPSION_2 0.01
#define EPSION_3 0.001

#define EPSION 0.00000001
#define GLEAF 0.5
#define HSPOT 0.5

#define GROUP_SIZEXY 8
#define GROUP_SIZEX 64
#define GROUP_SIZEXYZ 8

#define PID2 1.557963
#define TLASTNUM 8
#define DIFFUSE_ANGLE_NUM 32
#define BANDNUM 2162

#define NBAND 2002  // 2001 vnir+1 TIR 
#define RHOA 1.2047      // specific mass of air
#define CP   1004        // specific heat of dry air
#define KAPPA 0.4        // Von Karman constant
#define GEARTH  9.81     // gravity acceleration
#define AHC 119.7117122  //
#define MAIR 28.96       // molecular mass of dry air
#define MH20 18          // molecular mass of water
#define MCO2 44          // Molecular mass of carbon dioxide
#define SIGMASB 5.67e-8   // stefan boltzman constant
#define STRESS 1         // 
#define C2K    273.15    // melting point of water
#define KV 0.6396        // 
#define R  8.31           // Molar gas constant
#define LEAFG 0.5
#define CD   0.3         // drag coefficient for the vegetation
#define CR    0.35        // darg for the isolate tree
#define CD1 20.6       //fitting coefficients
#define PSICOR    0.2           // rOUGHNESS LAYER CORRECTION
#define CSSOIL 0.01 // DARY COEFFICIENT FOR SOIL

#define RAD_THRESHOLD 2    // radiance threshold
#define TMIN_THRESHOLD 220  // -50 C
#define TMAX_THRESHOLD 350  // +80 C


const vec3 DIFFUSE_DIRECTIONS[32] = {
    vec3(0.898904, 0.435512, 0.0479745),
    vec3(0.898904, -0.435512, -0.0479745),
    vec3(0.898904, 0.0479745, -0.435512),
    vec3(0.898904, -0.0479745, 0.435512),
    vec3(-0.898904, 0.435512, -0.0479745),
    vec3(-0.898904, -0.435512, 0.0479745),
    vec3(-0.898904, 0.0479745, 0.435512),
    vec3(-0.898904, -0.0479745, -0.435512),
    vec3(0.0479745, 0.898904, 0.435512),
    vec3(-0.0479745, 0.898904, -0.435512),
    vec3(-0.435512, 0.898904, 0.0479745),
    vec3(0.435512, 0.898904, -0.0479745),
    vec3(-0.0479745, -0.898904, 0.435512),
    vec3(0.0479745, -0.898904, -0.435512),
    vec3(0.435512, -0.898904, 0.0479745),
    vec3(-0.435512, -0.898904, -0.0479745),
    vec3(0.435512, 0.0479745, 0.898904),
    vec3(-0.435512, -0.0479745, 0.898904),
    vec3(0.0479745, -0.435512, 0.898904),
    vec3(-0.0479745, 0.435512, 0.898904),
    vec3(0.435512, -0.0479745, -0.898904),
    vec3(-0.435512, 0.0479745, -0.898904),
    vec3(0.0479745, 0.435512, -0.898904),
    vec3(-0.0479745, -0.435512, -0.898904),
    vec3(0.57735, 0.57735, 0.57735),
    vec3(0.57735, 0.57735, -0.57735),
    vec3(0.57735, -0.57735, 0.57735),
    vec3(0.57735, -0.57735, -0.57735),
    vec3(-0.57735, 0.57735, 0.57735),
    vec3(-0.57735, 0.57735, -0.57735),
    vec3(-0.57735, -0.57735, 0.57735),
    vec3(-0.57735, -0.57735, -0.57735)
};




#endif