#include "core/lifecycle.h"
#include "storage/overlay.h"
#include "core/cgroup.h"
#include "core/network.h"
#include "container_config.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <cstring>
#include <cerrno>

namespace Core{

    const int STACK_SIZE = 1024 * 1024;

    int container_main(void* args) {
        std::cout << "[Child] Namespace subsystem creation complete.\n";

        ContainerArgs* config = static_cast<ContainerArgs*>(args);
        
        const char* new_hostname = "minicontainer-bubble";
        if(sethostname(new_hostname, std::strlen(new_hostname)) == -1){
            std::cerr << "[Child] Namespace issue: Hostname configuration rejected: " << std::strerror(errno) << "\n";
        }


        std::string target_rootfs = "/workspaces/minicontainer/images/alpine";
        if (!Storage::jail_filesystem(target_rootfs)) {
            std::cerr << "[Child] Storage initialization sequence breached. Terminating runtime.\n";
            return 1;
        }

        std::vector<char*> exec_args;
        for(const auto& arg : config->command){
            exec_args.push_back(const_cast<char*>(arg.c_str()));
        }
        exec_args.push_back(nullptr);
        
        std::cout << "[Child] Relinking system control flow. Spawning inner container environment...\n\n";
        if(execvp(exec_args[0], exec_args.data()) == -1){
            std::cerr << "[Child] Execution map link broken: /bin/sh allocation failure: " << std::strerror(errno) << "\n";
            return 1;
        }
        
        return 0;
    }

    void start_container(const ContainerArgs& args){
        std::cout << "[Parent] Allocating container runtime stack allocation map...\n";
        
        char* stack = new char[STACK_SIZE];
        char* stack_top = stack + STACK_SIZE;

        std::cout << "[Parent] Issuing system call: clone() to fork execution context with new isolation profiles...\n";

        pid_t child_pid = clone(
            container_main, 
            stack_top, 
            CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD, 
            const_cast<void*>(static_cast<const void*>(&args))
        );

        if(child_pid == -1){
            std::cerr << "[Parent] Critical: Process allocation failed inside clone(): " 
                      << std::strerror(errno) << "\n";
            delete[] stack;
            return;
        }

        std::cout << "[Parent] Container process initialized natively on host with PID: " << child_pid << "\n";


        if(!Network::setup_network(child_pid)){
            std::cerr<<"[Parent] Network setup failed.\n";
        }

        if(!Cgroup::apply_limits(child_pid, "26214400")){
            std::cerr << "[Parent] Resource allocation failed. Aborting runtime monitor.\n";
        }


        std::cout << "[Parent] Thread blocking active. Waiting for child exit state confirmation...\n";
        waitpid(child_pid, nullptr, 0);
        
        Cgroup::cleanup();
        Network::cleanup_network();

        std::cout << "\n[Parent] Inner shell has exited cleanly. Clearing dynamic memory allocations.\n";
        delete[] stack;
    }
}