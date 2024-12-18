//
// Created by ShIP on 2024/12/2.
//

#ifndef FIELD_BUFFER_H
#define FIELD_BUFFER_H

#include <nvvk/buffers_vk.hpp>
#include "buildingshadowio.h"


class Buffer {
public:
    Buffer()= default;
    bool createBuffer(std::shared_ptr<BuildingShadowIO> &modelio);
    void destroy(std::shared_ptr<BuildingShadowIO> &modelio);

};

#endif //BUFFER_H
