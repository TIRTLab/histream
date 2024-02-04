//
// Created by admin on 2024/1/24.
//

#ifndef FIELD_COMPO_H
#define FIELD_COMPO_H

#include "src/raytracing/raytracingio.h"
#include "src/voxellst/voxellstio.h"
#include "fileio.h"
#include "src/base/utils.h"

class Compo {

public:
    Compo(){};

    bool createCompOptical(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &raytracingio);

    bool createCompProperty(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxellstIO> &voxellstio);

    float calctav(float alfa,float nr);
    void fluspect(OptCoeff fluspectCoeff, FluspectParam fluspectParam, std::vector<Spectral>& spectrals);


    void bsm(OptCoeff bsmCoeff, BSMParam bsm, std::vector<Spectral> &spectral);

};


#endif //FIELD_COMPO_H
