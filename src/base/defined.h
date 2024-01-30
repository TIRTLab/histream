//
// Created by admin on 2024/1/28.
//

#ifndef FIELD_DEFINED_H
#define FIELD_DEFINED_H

// shader
// coefficient


#include "structs.h"

struct FluspectCoeff
{
//    float * nr_, *kdm_, *kab_, *kw_, *ks_, *phiI_, *phiII_;
    std::vector<float> nr_;
    std::vector<float> kdm_;
    std::vector<float> kab_;
    std::vector<float> kw_;
    std::vector<float> ks_;
    std::vector<float> phiI_;
    std::vector<float> phiII_;

};


class Defined {
public:

    Defined(){};

    std::string definedDir = "";
    FluspectCoeff m_fluspectCoeff;

    std::shared_ptr<nvvk::Buffer> m_pAeroBuffer;    // Aero
    std::vector<AeroCoeff> aerocoeffs;


};


#endif //FIELD_DEFINED_H
