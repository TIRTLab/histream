

#pragma once
#include <iostream>


#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE


#include "src/base/engine.h"


using namespace std;

int main(int argc, char **argv) {

//    std::string projectOutDir;
//    std::string version;

//    argc = 5;
//    argv[0] = "D:\\code\\field_n\\bin_x64\\Debug\\field.exe";
//    argv[1] = "-w";
//    argv[2] = "D:\\rash\\tst11eb";
//    argv[3] = "-v";
//    argv[4] = "eVoxelEB";


//    argc = 5;
//    argv[0] = "D:\\code\\field_n\\bin_x64\\Release\\field.exe";
//    argv[1] = "-w";
//    argv[2] = "D:\\rash\\tst11eb";
//    argv[3] = "-v";
//    argv[4] = "eVoxelEB";

//    // 输出所有传入的命令行参数，帮助调试
//    std::cout << "Number of arguments: " << argc << std::endl;
//    for (int i = 0; i < argc; i++) {
//        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
//    }
//
//    int index = 0;
//    // 遍历命令行参数查找 -w 参数
//    for (int i = 1; i < argc; i++) {
//        std::string arg = argv[i];
//
//        if (arg == "-w" && i + 1 < argc) {
//            projectOutDir = argv[i + 1];
////            break;
//            index += 1;
//        }
//
//        if (arg == "-v" && i + 1 < argc) {
//            version = argv[i + 1];
////            break;
//            index += 1;
//        }
//        if (index == 2){
//            break;
//        }
//
//        if (i == argc -1){
//            return false;
//        }
//    }
    std::string projectOutDir = "D:\\rash\\tst16voxelrt";
    std::string version = "eVoxelRT";
    std::string path = projectOutDir;
    std::string V = version;
    if (V == ""){
        return false;
    }
    Engine engine;
    engine.input(path, V);
    engine.create();
    engine.run();
    engine.destroy();

    return true;
}
