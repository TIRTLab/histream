

#pragma once
#include <iostream>


#include <vulkan/vulkan.hpp>

#include "nvh/inputparser.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE


#include "src/base/engine.h"


using namespace std;

int main(int argc, char **argv) {
    const InputParser parser(argc, argv);
    const string path = parser.getString("-w", "");
    const string version = parser.getString("-v", "");

     // std::string path = "D:/data/field_app/example1";
     // path = "D:/data/hyperSpectral/test1";
     // std::string version = "eRaytracing";

    // std::string path = "D:/data/field_app/voxelRT";
    // std::string version = "eVoxelRT";

    // std::string path = "D:/data/field_app/voxelEB_release";
    // std::string version = "eVoxelEB";

//     std::string path = "D:/test2";
//     std::string version = "eVoxelEB";
//
    if (version.empty() || path.empty()) {
        cout << "Wrong Input" << endl;
        return true;
    }

    Engine engine;
    engine.input(path, version);
    engine.create();
    engine.run();
    engine.destroy();

    return false;
}
