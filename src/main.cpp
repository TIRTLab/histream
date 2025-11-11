

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
    const string xmlPath = parser.getString("-f", "");  //// 批量模式Batch mode,给定xml的路径
    std::string filePath;


    // std::string xmlPath = "D:/data/hiStream/night_correction/Input.xml";
    // std::string version = "eVoxelEB";
    // std::string path;
    // std::string filePath;


    if (version.empty())
    {
        cout << "Wrong version input" << endl;
        return true;
    }
    if (!xmlPath.empty())
    {
        filePath = xmlPath;
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
