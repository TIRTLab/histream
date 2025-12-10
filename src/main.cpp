

#pragma once
#include <iostream>


#include <vulkan/vulkan.hpp>

#include "nvh/inputparser.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE


#include "src/base/engine.h"


using namespace std;

int main(int argc, char **argv) {
    const InputParser parser(argc, argv);
    const string path = parser.getString("-w", "");     //// 路径，单独运行时的文件夹
    const string version = parser.getString("-v", "");  ////
    std::string filePath;

    //
    // std::string version = "eVoxelization";
    // std::string path= "D://data//hiStream//Voxelization";
    // std::string filePath;

    if (version.empty())
    {
        cout << "Wrong version input" << endl;
        return true;
    }
    if (!path.empty())
    {
        filePath = path + "//Input.xml";
    }

    Engine engine;
    engine.input(filePath, version);
    engine.create();
    engine.run();
    engine.destroy();

    return false;
}
