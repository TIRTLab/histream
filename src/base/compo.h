//
// Created by admin on 2024/1/24.
//

#ifndef FIELD_COMPO_H
#define FIELD_COMPO_H

#include "src/raytracing/raytracingio.h"
#include "src/voxeleb/voxelebio.h"
#include "src/voxelrt/voxelrtio.h"
#include "fileio.h"
#include "src/base/utils.h"

class Compo {

public:
    Compo(){};

    bool createCompOptical(std::shared_ptr<FileIO> &fileio, std::shared_ptr<RaytracingIO> &raytracingio);
    bool createCompProperty(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelebIO> &voxelebio);
    bool createCompOptical(std::shared_ptr<FileIO> &fileio, std::shared_ptr<VoxelrtIO> &voxelrtio);

    bool createCompoOptical(std::shared_ptr<FileIO> &fileio);

    /// fluspect model
    float calctav(float alfa,float nr);
    void fluspect(OptCoeff fluspectCoeff, FluspectParam fluspectParam, std::vector<Spectral>& spectrals);
    // bsm model
    void bsm(OptCoeff bsmCoeff, BSMParam bsm, std::vector<Spectral> &spectral);


//private:
    std::vector<Canopy> canopies;
    std::vector<LeafBio> leafbios;
    std::vector<SoilSet> soilsets;
    std::vector<BuildUp> buildups;
    std::vector<Spectral> spectrals;
    std::vector<Thermal> thermals;
    std::vector<Spectral> fixedSpectrals;

    std::map<std::string,int>  spectralNames;
    std::map<std::string,int>  thermalNames;
    std::map<std::string,int> canopyNames;
    std::map<std::string,int> leafbioNames;
    std::map<std::string,int> soilsetNames;
    std::map<std::string,int> aeroNames;


};


#endif //FIELD_COMPO_H
