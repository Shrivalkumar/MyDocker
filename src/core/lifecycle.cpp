#include "core/lifecycle.h"
#include "storage/overlay.h"
#include "core/cgroup.h"
#include "core/network.h"
#include "container_config.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <cstring>
#include <cerrno>

namespace Core {

    const int STACK_SIZE = 1024 * 1024;

    int container_main(void* args) {
        auto* dynamic_args = static_cast<std::pair<const ContainerArgs*, int*>*>(args);
        const ContainerArgs* config = dynamic_args->first;
        int* sync_pipe = dynamic_args->second;

       
        close(sync_pipe[1]);

        //here we are blocking the read so that it wait until the parent writes the data to the pip which eliminates the race condition 
        char ch;
        read(sync_pipe[0], &ch, 1);
        close(sync_pipe[0]); 

        std::cout << "[Child] Parent signaling received. Advancing state...\n";
        
        const char* new_hostname = "minicontainer-bubble";
        if (sethostname(new_hostname, std::strlen(new_hostname)) == -1) {
            std::cerr << "[Child] Namespace issue: Hostname configuration rejected\n";
        }

        std::string target_rootfs = "/workspaces/minicontainer/images/alpine";
        if (!Storage::jail_filesystem(target_rootfs)) {
            std::cerr << "[Child] Storage initialization sequence breached.\n";
            return 1;
        }

        std::vector<char*> exec_args;
        for (const auto& arg : config->command) {
            exec_args.push_back(const_cast<char*>(arg.c_str()));
        }
        exec_args.push_back(nullptr);
        
        if (execvp(exec_args[0], exec_args.data()) == -1) {
            std::cerr << "[Child] Execution map link broken: " << std::strerror(errno) << "\n";
            return 1;
        }
        
        return 0;
    }

    void start_container(const ContainerArgs& args) {
        std::cout << "[Parent] Allocating container runtime stack allocation map...\n";
        
        char* stack = new char[STACK_SIZE];
        char* stack_top = stack + STACK_SIZE;

        // Allocate synchronization pipe control array
        int sync_pipe[2];
        if (pipe(sync_pipe) < 0) {
            std::cerr << "[Parent] Internal tracking pipeline allocation failure.\n";
            delete[] stack;
            return;
        }

        std::pair<const ContainerArgs*, int*> passing_payload(&args, sync_pipe);

        pid_t child_pid = clone(
            container_main, 
            stack_top, 
            CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD, 
            &passing_payload
        );

        if (child_pid == -1) {
            std::cerr << "[Parent] Critical: Process allocation failed inside clone()\n";
            close(sync_pipe[0]);
            close(sync_pipe[1]);
            delete[] stack;
            return;
        }

        // Close read channel descriptor on the parent side context
        close(sync_pipe[0]);

        std::cout << "[Parent] Container process initialized natively on host with PID: " << child_pid << "\n";

        
        if(!Network::setup_network(child_pid)) {
            std::cerr << "[Parent] Network setup failed.\n";
        }

        if(!Cgroup::apply_limits(child_pid, "26214400")){
            std::cerr << "[Parent] Resource allocation failed.\n";
        }

        //The Unclock
        write(sync_pipe[1], "x", 1);
        close(sync_pipe[1]);

        std::cout << "[Parent] Thread blocking active. Waiting for child exit state confirmation...\n";
        waitpid(child_pid, nullptr, 0);
        
        Cgroup::cleanup();
        Network::cleanup_network();

        std::cout << "\n[Parent] Inner shell has exited cleanly. Clearing dynamic memory allocations.\n";
        delete[] stack;
    }
}