

#pragma once
#include <iostream>


#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#
#include "src/base/engine.h"


using namespace std;

int main(int argc, char **argv) {



    int dateinfo = 20230101, timeinfo = 40000;
    float vza = 0, vaa = 0, sza = 40, saa = 0;
    float Tsunlitroof = 320, Tshadedroof = 300, Tsunlitstreat = 320, Tshadedstreat = 300, Tsunlitwall = 320, Tshadedwall = 300, Tveg = 297;

    for (int k = 0; k < 1; k++) {



        Engine engine;
        engine.input(" ");


        engine.create();
        engine.run();
        engine.destroy();

    }
    return 1;
}
