//
// Created by admin on 2024/1/24.
//

#include "geometry.h"


bool Geometry::createGeometry(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &raytracingio){

//    auto & sceneio = raytracingio->m_sceneio;





    //--------------------------------------------------------
    //--- Resolution
    //-------------------------------------------------------
    raytracingio->resolution = fileio->m_pRaytracingXml->sensorxml.resolution;
    CameraManip.setWindowSize(raytracingio->resolution.x,raytracingio->resolution.y );
    //---------------------------------------------------------
    // Angles
    //---------------------------------------------------------
    float vza, vaa, sza, saa;
    LightXml lightxml = fileio->m_pRaytracingXml->lightxml;
    SensorXml sensorxml = fileio->m_pRaytracingXml->sensorxml;




    sza = lightxml.solarAngle[0];
    saa = lightxml.solarAngle[1];
    for (int i = 0; i < sensorxml.viewAngles.size(); i++)
    {
        vza = sensorxml.viewAngles[i][0];
        vaa = sensorxml.viewAngles[i][1];
        raytracingio->angles.emplace_back(glm::vec4{vza, vaa, sza, saa});
    }
    //raytracingio->n_angle = sensorxml.viewAngles.size();
    //---------------------------------------------------------
    // Bands
    //---------------------------------------------------------
    raytracingio->waves = fileio->m_pRaytracingXml->sensorxml.waves;

    //---------------------------------------------------------
    // LIGHT AND SENSOR INI with Angle 0 and Band 0
    //---------------------------------------------------------
    vza = raytracingio->angles[0].x;
    vaa = raytracingio->angles[0].y;
    sza = raytracingio->angles[0].z;
    saa = raytracingio->angles[0].w;
    raytracingio->sensor = createSensor(raytracingio->sceneSize,raytracingio->sceneOrigin, vza, vaa, 1.0);
    raytracingio->light = createLight(sza, saa, fileio->m_pRaytracingXml->lightxml.direct, fileio->m_pRaytracingXml->lightxml.diffuse,
                                      fileio->m_pRaytracingXml->lightxml.solarTemperature,
                                      fileio->m_pRaytracingXml->lightxml.skyTemperature);

    return true;

}

LightSet Geometry::createLight(float sza, float saa, float direct, float diffuse,float solarT,float skyT)
{
    LightSet light;
    if (sza == 0.0 || sza == 45.0) sza = sza + ANGLE_COR;
    float r = SENSOR_HEIGHT;
    float rd = DEG2RAD;
    glm::vec3 origin = glm::vec3(0, 0, 0);
    glm::vec3 lightPos = glm::vec3(r * std::sin(sza * rd) * std::cos(saa * rd),
                                           r * std::cos(sza * rd), r * std::sin(sza * rd) * std::sin(saa * rd));

    light.direction = lightPos;
    light.direct = direct;
    light.diffuse = diffuse;
    light.skyTemperature = skyT;
    light.solarTemperature = solarT;

    return light;
}

SensorMatrix Geometry::createSensor(glm::vec3 scenesize, glm::vec3 sceneorigen, float vza, float vaa, float ratio) {

   // auto & sceneio = raytracingio->m_sceneio;

    SensorMatrix sensor;
    if (vza == 0.0 || vza == 45.0) vza = vza + ANGLE_COR;
    float r = SENSOR_HEIGHT;
    float rd = DEG2RAD;
    glm::vec3 origin = glm::vec3(0, 0, 0);
    glm::vec3 sensorPos = glm::vec3(r * std::sin(vza * rd) * std::cos(vaa * rd),
                                            r * std::cos(vza * rd), r * std::sin(vza * rd) * std::sin(vaa * rd));
    CameraManip.setFov(SENSOR_FOV);
    CameraManip.setLookat(sensorPos, origin, glm::vec3(0, 1, 0));
    float fovv = CameraManip.getFov();



    glm::vec3 semi = { scenesize.x / 2.0, 0, scenesize.y / 2.0 };
    glm::vec3 dimensionMin = -semi + glm::vec3{ sceneorigen.x, 0, sceneorigen.y };
    glm::vec3 dimensionMax = semi + glm::vec3{ sceneorigen.x, 0, sceneorigen.y };

    //float scale = m_pRaytracingXml->scene.stepSize;
    dimensionMin.y = 0;
    dimensionMax.y = 0;
    //CameraManip.fit(dimensionMin * ratio / scale, dimensionMax * ratio / scale); // the sensor position height is changed.
    CameraManip.fit(dimensionMin * ratio, dimensionMax * ratio); // the sensor position height is changed.


    float width = CameraManip.getWidth();
    float height = CameraManip.getHeight();
    const float aspectRatio = CameraManip.getWidth() / static_cast<float>(CameraManip.getHeight());
    glm::mat4 view = CameraManip.getMatrix();
    nvmath::mat4f projj = nvmath::perspectiveVK(CameraManip.getFov(), aspectRatio, 0.0001f, 10000.0f);
    //glm::mat4 proj = glm::perspective(CameraManip.getFov(), aspectRatio, 0.0001f, 10000.0f);
    glm::mat4 proj = glm::perspectiveRH_ZO(glm::radians(CameraManip.getFov()), aspectRatio, 0.1f, 1000.0f);
    proj[1][1] *= -1;
    sensor.viewInverse = glm::inverse(view);
    sensor.projInverse = glm::inverse(proj);
    //nvmath::vec3f eye, center, up;

    glm::vec3 eye, center,up;
    CameraManip.getLookat(eye, center, up);                             // get sensor (eye) and center.

    float fov = CameraManip.getFov();
    sensor.focalDist = glm::length(center - eye);
    sensor.aperture = 0.0;
    sensor.direction = eye - center;

  //  sensor.n_wave = raytracingio->waves.size();

    return sensor;
}


void Geometry::updateSensor( std::shared_ptr<RaytracingIO> &raytracingio, SensorMatrix &sensor){


    auto &m_pBufferSensor = raytracingio->m_pBufferSensor;
    auto &m_device = raytracingio->m_device;
    auto &m_queueIndex = raytracingio->m_queueIndex;

    nvvk::CommandPool cmdBufGet(m_device, m_queueIndex);
    vk::CommandBuffer cmdBuf = cmdBufGet.createCommandBuffer();
    vkCmdUpdateBuffer(cmdBuf, (*m_pBufferSensor).buffer, 0, sizeof(SensorMatrix), &sensor);
    cmdBufGet.submitAndWait(cmdBuf);
}


void Geometry::updateLight(std::shared_ptr<RaytracingIO> &raytracingio, LightSet &light){


    auto &m_pBufferLight = raytracingio->m_pBufferLight;
    auto &m_device = raytracingio->m_device;
    auto &m_queueIndex = raytracingio->m_queueIndex;

    nvvk::CommandPool cmdBufGet(m_device, m_queueIndex);
    vk::CommandBuffer cmdBuf = cmdBufGet.createCommandBuffer();
    vkCmdUpdateBuffer(cmdBuf, (*m_pBufferLight).buffer, 0, sizeof(LightSet), &light);
    cmdBufGet.submitAndWait(cmdBuf);
}