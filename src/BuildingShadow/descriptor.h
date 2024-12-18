//
// Created by ShIP on 2024/12/2.
//

#ifndef FIELD_DESCRIPTOR_H
#define FIELD_DESCRIPTOR_H

#include "buildingshadowio.h"
class Descriptor{
public:
    enum ShadowBindingInd
    {
        facetInfo,
        tlas,
        meshLink,
        instanceLink,
    };

    bool createDescriptor(std::shared_ptr<BuildingShadowIO> &modelio);
    void destroy(std::shared_ptr<BuildingShadowIO> &modelio);

};

#endif //DESCRIPTOR_H
