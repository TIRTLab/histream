

#pragma once
#include <iostream>


#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#
#include "src/base/engine.h"


using namespace std;

int main(int argc, char **argv)
{

    std::string path = "test";

    Engine engine;
    engine.input(path);
    engine.create();
    engine.run();
    engine.destroy();

	return 1;
}
