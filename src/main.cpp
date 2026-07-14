#include "../include/core/lifecycle.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "===========================================\n";
    std::cout << "=== MiniContainer Engine Lifecycle Boot ===\n";
    std::cout << "===========================================\n";

    Core::ContainerArgs config;

    if(argc < 2){
        config.command.push_back("/bin/sh");
    }else{
        for(int i=1;i<argc;i++){
            config.command.push_back(argv[i]);
        }
    }


    Core::start_container(config);
    
    std::cout << "=== MiniContainer Engine Gracefully Halted ===\n";
    return 0;
}