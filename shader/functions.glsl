#ifndef FUNCTIONS_GLSL
#define FUNCTIONS_GLSL

#include "global.glsl"
#include "parameters.h"
//--------------------------------------------------------------------------------------------
// Common functions
//--------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// Avoiding self intersections (see Ray Tracing Gems, Ch. 6)
//-------------------------------------------------------------------------------------------------
vec3 OffsetRay(in vec3 p, in vec3 n)
{
  const float intScale   = 256.0f;
  const float floatScale = 1.0f / 65536.0f;
  const float origin     = 1.0f / 32.0f;

  ivec3 of_i = ivec3(intScale * n.x, intScale * n.y, intScale * n.z);

  vec3 p_i = vec3(intBitsToFloat(floatBitsToInt(p.x) + ((p.x < 0) ? -of_i.x : of_i.x)),
                  intBitsToFloat(floatBitsToInt(p.y) + ((p.y < 0) ? -of_i.y : of_i.y)),
                  intBitsToFloat(floatBitsToInt(p.z) + ((p.z < 0) ? -of_i.z : of_i.z)));

  return vec3(abs(p.x) < origin ? p.x + floatScale * n.x : p_i.x,  //
              abs(p.y) < origin ? p.y + floatScale * n.y : p_i.y,  //
              abs(p.z) < origin ? p.z + floatScale * n.z : p_i.z);
}

//-------------------------------------------------------------------------------------------------
// From tangent space/local space to world space and from world space to tangent space/ local space
//-------------------------------------------------------------------------------------------------
vec3 World2Tangent(vec3 normal,vec3 tangent,vec3 binormal,vec3 v)
{
    return vec3(dot(tangent,v),dot(binormal,v),dot(normal,v));
}
vec3 Tangent2World(vec3 normal,vec3 tangent,vec3 binormal,vec3 v)
{
    //return vec3(dot(tangent,v),dot(binormal,v),dot(normal,v));
    return vec3(dot(binormal,v),dot(tangent,v),dot(normal,v));
}


// Ray-Sphere intersection
// http://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
float hitSphere(const Sphere s, const Ray r)
{
  vec3  oc           = r.origin - s.center;
  float a            = dot(r.direction, r.direction);
  float b            = 2.0 * dot(oc, r.direction);
  float c            = dot(oc, oc) - s.radius * s.radius;
  float discriminant = b * b - 4 * a * c;
  if(discriminant < 0)
  {
    return -1.0;
  }
  else
  {
    return (-b - sqrt(discriminant)) / (2.0 * a);
  }
}

// Ray-AABB intersection
float hitAabb(const AllAabb aabb, const Ray r)
{
  vec3  invDir = 1.0 / r.direction;
  vec3  tbot   = invDir * (aabb.minimum - r.origin);
  vec3  ttop   = invDir * (aabb.maximum - r.origin);
  vec3  tmin   = min(ttop, tbot);
  vec3  tmax   = max(ttop, tbot);
  float t0     = max(tmin.x, max(tmin.y, tmin.z));
  float t1     = min(tmax.x, min(tmax.y, tmax.z));
  if(t1 > max(t0, 0.0))
  {
    if(t0<=0) return t1;
    else return t0;
  }
  else return -1.0;
//   return t1 > max(t0, 0.0) ? t0 : -1.0;
}

//--------------------------------------------------------------------------------------------
//  SATE VAPOR USING A TEMPERATURE  T 300K
//--------------------------------------------------------------------------------------------
float es_fun(float T)
{
    float a = 7.5;
    float b = 237.3;
    float temp = a*T/(b+T);
    return 6.107*pow(10,temp);
}
//slope of the saturated pressure function
float s_fun(float es, float T)
{
    return es*2.3026*7.5*237.3/((237.3+T)*(237.3+T));
}


//-------------------------------------------
// thermal inertial
//-------------------------------------------
float thermal_inertial(float cs,float rhos,float lambdas)
{
    return sqrt(cs*rhos*lambdas);
}


vec2 BallBerry(float Cs, float RH,float A,float BallBerrySlope, float minCi,float CiInput)
{
    float Ci = max(minCi * Cs, Cs *(1-1.6/(BallBerrySlope * RH)) );
    float gs = 0;
    return vec2(Ci,gs);
}

//----------------------------------------------------------
// quadratic formula, root of least magnitude: AX2 + BX + C = 0
//    for the eqn ax^2 + bx + c, 
//    if dsign is:
//       -1, 0: choose the smaller root
//       +1: choose the larger root
//----------------------------------------------------------

float sel_root(float a,float b,float c,float design)
{
    float x;
    if(a ==0)
    {
        x = -c/b;
    }else
    {
        if(design ==0)
        {
            design = -1;
        }
        x = (-b + design *sqrt(b*b - 4*a*c))/(2*a);
    }
    return x;
}

float computeA(float Ci, int Type, float g_m,float Vs_C3,float MM_consts,float Rd,float Vcmax,
                float Gamma_star, float Je,float effcon, float atheta,float kpepcase)
{
    float Vc,Ve,Vs;
    if(Type == 3)
    {
        float Vs = Vs_C3;
        float Vc = Vcmax *(Ci-Gamma_star)/(MM_consts + Ci);
        float CO2_per_electron =(Ci-Gamma_star)/(Ci+2*Gamma_star) * effcon;
        float Ve = Je * CO2_per_electron;
    }else if(Type ==4)
    {
        float Vc = Vcmax;
        float Vs = kpepcase * Ci;
        float CO2_per_electron = effcon;
        float Ve = Je * CO2_per_electron;
    }

    float V = sel_root(atheta, -(Vc+Ve), Vc*Ve, 1);
    float Ag = sel_root(0.98,-(V+Vs),V*Vs, -1);
    float A = Ag - Rd;

    return A;
}




//--------------------------------------------------------------------------------------------
// planck and invplanck functions to convert temperature to radiance
//--------------------------------------------------------------------------------------------
 float Planck(float wavelength, float temperature)
{
    float c1 = 11910.439340652*10000;
    float c2 = 14388.291040407;
    if(wavelength > 50) wavelength = wavelength / 1000.0;
    // double c1 = 11910.439340* 10000;
    // double c2 = 14388.291040;

    //if (temperature < 100) temperature = temperature + 273.15;
    double radiance = c1 / (pow(wavelength, 5) * (exp(c2 / temperature / wavelength) - 1));
    
    return float(radiance);
}

 float PlanckT(float wavelength, float temperature)
{
    float c1 = 11910.439340652;
    float c2 = 14388.291040407;
    if(wavelength > 50) wavelength = wavelength / 1000.0;
    // double c1 = 11910.439340* 10000;
    // double c2 =14388.291040;

    double radiance = c1 / (pow(wavelength, 5) * (exp(c2 / temperature / wavelength) - 1)) ;
    
    return float(radiance);
}

float InvPlanck(float wavelength,float radiance)
{
    // double c1 = 11910.439340 * 10000; 
    // double c2 = 14388.291040;
    float c1 = 11910.439340 * 10000; 
    float c2 = 14388.291040;
    if(wavelength > 50) wavelength = wavelength / 1000.0;
    // double temp = c1 / (radiance * pow((wavelength),5)) + 1;
    // double tempp = c2 / (wavelength * log(temp));
    float temp = c1 / (radiance * pow((wavelength),5)) + 1;
    float tempp = c2 / (wavelength * log(temp));
    return float(tempp);
}

//--------------------------------------------------------------------------------------------
//  hapke directional emissivity. {0 < w < 1; 1 < K < 10}
//--------------------------------------------------------------------------------------------
float Emissivity_Hapke(float cosTheta, float w, float K)
{
    float emissivity;
    emissivity = sqrt(1-w) * (1 + 2 * cosTheta/K)/(1 + 2 * cosTheta/K * sqrt(1-w));
    return emissivity;
}


//--------------------------------------------------------------------------------------------
//  From temperature to radiance T i.e. 300 K
//--------------------------------------------------------------------------------------------
float Stefen_Boltzmann(float T,float emis)
{
    return T/100*T/100*T/100*T/100*emis*5.6704;
}

//-------------------------------------------------------------------------------------------------
//  Return the tangent and binormal from the incoming normal
//-------------------------------------------------------------------------------------------------
int createCoordinateSystem(in vec3 N, out vec3 Nt, out vec3 Nb)
{
  Nt = normalize(((abs(N.z) > 0.99999f) ? vec3(-N.x * N.y, 1.0f - N.y * N.y, -N.y * N.z) :
                                          vec3(-N.x * N.z, -N.y * N.z, 1.0f - N.z * N.z)));
  Nb = cross(Nt, N);
  return 1;
}

//--------------------------------------------------------------------------------------------
// Vector Distance
//--------------------------------------------------------------------------------------------
float Vector2Distance(vec3 vector)
{
    float pathLength = sqrt(vector.x*vector.x+vector.y*vector.y+vector.z*vector.z);
    return pathLength;
}


//--------------------------------------------------------------------------------------------
// hotspot function proposed by Kuusk 
//--------------------------------------------------------------------------------------------

// get passed voxel ID (ivec3)
ivec3 getPassedVoxelId(vec3 position, float scale,vec3 direction)
{
    float minTemp = 0.001;
    int xb = int(floor((position.x-direction.x*minTemp *scale)/scale));
    int yb = int(floor((position.y-direction.y*minTemp *scale)/scale));
    int zb = int(floor((position.z-direction.z*minTemp *scale)/scale)); 

    ivec3 voxelId = ivec3(xb,yb,zb);
    return voxelId;
}

// WITH SEMI
// foward direction 
ivec3 getVoxelIdSemi(vec3 position,vec3 semiRange, float scale,vec3 direction, out ivec3 alignVoxelId)
{
    float minTemp = 0.05;

    int xf = int((position.x+semiRange.x+direction.x*minTemp*scale)/scale);
    int yf = int((position.y+direction.y*minTemp*scale)/scale);
    int zf = int((position.z+semiRange.z+direction.z*minTemp*scale)/scale); 


    int xb = int((position.x+semiRange.x-direction.x*minTemp *scale)/scale);
    int yb = int((position.y-direction.y*minTemp*scale)/scale);
    int zb = int((position.z+semiRange.z-direction.z*minTemp*scale)/scale); 

    ivec3 voxelId = ivec3(xb,yb,zb);
    alignVoxelId = ivec3(xf,yf,zf);
    
    return voxelId;
}

ivec3 getVoxelIdSimple(vec3 position,vec3 semiRange, float scale)
{

    int x = int((position.x+semiRange.x)/scale);
    int y = int(position.y/scale);
    int z = int((position.z+semiRange.z)/scale);
    ivec3 voxelId = ivec3(x,y,z);
    return voxelId;
}


int SurfFlatten(ivec3 voxelId, ivec3 voxelRes)
{
    int surf1DId = voxelId.x + voxelId.z * voxelRes.x;
    return surf1DId;
};

// uint SurfFlatten(ivec3 voxelId, ivec3 voxelRes)
// {
//     uint surf1DId = voxelId.x*voxelRes.z + voxelId.z ;
//     return surf1DId;
// };

//--------------------------------------------------------------------------------------------
// Geometry function
//--------------------------------------------------------------------------------------------
// wo: from camera to position
// wi: from position to sun
// if negative, the same side; vice versa

// bool isSameSide(vec3 wi,vec3 wo,vec3 normal)
// {
//     float wio = dot(wi,normal)*dot(wi,normal);
//     if(wio <= 0) return false;
//     else return true;
// }
bool IsSameSide(vec3 wi,vec3 wo,vec3 normal)
{
    float wio = dot(wi,normal)*dot(wo,normal);
    if(wio < 0) return true;
    else return false;
}


float atan2(float y,float x)
{
    float phi;
    if(x > 0)
    {
        phi = atan(y/x);
    }else if(x<0 && y>=0)
    {
        phi = atan(y/x) + PI;
    }else if(x<0 && y<0)
    {
        phi = atan(y/x) - PI;
    }else if(x == 0 && y>0)
    {
        phi = PI/2.0;
    }else if( x==0 && y<0)
    {
        phi = -PI/2.0;
    }else
    {
        phi = 0;
    }
    return phi;
}

// // a direction
// vec2 scattering_phase_function_direct_XYZ(vec3 directioni,vec3 directionj)
// {
//     vec2 Gamma;
//     float thetai = acos(directioni.z);
//     float thetaj = acos(directionj.z);
//     float alphai = 0,alphaj = 0;
//     alphai = atan2(directioni.y,directioni.x);
//     alphaj = atan2(directionj.y,directionj.x);
//     float cosa = directioni.z * directionj.z + sin(thetai) * sin(thetaj)*cos(alphai-alphaj);
//     float a = acos(cosa);
//     float sina = sin(a);
//     Gamma.x = abs(sina + (PI - a)*cosa)/(3*PI);
//     Gamma.y = abs(sina + a*cosa)/(3*PI);
    
//     return Gamma;
// }

float cross0(vec3 A, vec3 B){
    return A.x*B.x+A.y*B.y+A.z*B.z;
}

vec2 scattering_phase_function_direct_XZY(vec3 directioni,vec3 directionj)
{
    vec2 Gamma;
    float thetai = acos(directioni.y);
    float thetaj = acos(directionj.y);
    float alphai = 0,alphaj = 0;
    alphai = atan2(directioni.z,directioni.x);
    alphaj = atan2(directionj.z,directionj.x);
    //入射方向与出射方向的夹角余弦值; Cosine value of scattering angle between incident direction and exit direction
    float cosa = directioni.y * directionj.y + sin(thetai) * sin(thetaj)*cos(alphai-alphaj);
    //scattering angle 
    float a = acos(cosa);
    float sina = sin(a);
    // Gamma.x = abs(sina + (PI - a)*cosa)/(3*PI);
    // Gamma.y = abs(sina + a*cosa)/(3*PI);
    Gamma.x = (sina - a * cosa) / 3 * PI;
    Gamma.y = cosa / 3;
    return Gamma;
}

float Hotspot_Kuusk(vec3 directioni,vec3 directionj, float pl_from, float pl_to, float density, float hs)
{

    float thetai = acos((directioni.y));
    float thetaj = acos((directionj.y));
    float alphai = 0,alphaj = 0;
    alphai = atan2(directioni.z,directioni.x);
    alphaj = atan2(directionj.z,directionj.x);
    float cosa = (directioni.y) * (directionj.y) + sin(thetai) * sin(thetaj)*cos(alphai-alphaj);
    float angle = acos(cosa);
    float sina = sin(angle);

    float pl = pow(pl_from,2)+pow(pl_to,2)-cosa*2.0*pl_from*pl_to;
    float correction = sqrt(pl)/hs;
    float xtemp = 1.0;
    if (correction <= 0) { xtemp = 1- correction * 0.5;}
    if (correction > 0) {xtemp = (1-exp(-correction))/correction;}
    float chs = sqrt(pl_from * pl_to) * xtemp;
    float xx = (chs - pl_from - pl_to) *GLEAF;
    float hotspot = exp(xx * density);

    return hotspot;
}

#endif 