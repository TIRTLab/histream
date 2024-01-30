//
// Created by admin on 2024/1/25.
//

#ifndef FIELD_DESCRIPTOR_H
#define FIELD_DESCRIPTOR_H

#include "voxellstio.h"

class Descriptor {
public:

    enum VoxellstbindingInd
    {
        spectral,
        fixedSpectral,
        thermal,
        tempe,
        canopy,
        instanceLink,
        voxelLink,
        nano,
        tlas,
        sensor,
        wave,
        light,
        waveset,
        dir,
        rads,
        netRad,
        pnet,
        storage,
        aero,
        meteo,
        surfL,
        raa,
        leafBio,
        soilSet,
        rss,
        air,
        flux,
        tLast,
        state,
    };


    bool createDescriptor(std::shared_ptr<VoxellstIO> &raytracingio);
    void destroy(std::shared_ptr<VoxellstIO> &raytracingio);

};


#endif //FIELD_DESCRIPTOR_H
