#include "core/cgroup.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

namespace Core{
    namespace Cgroup{
        const std::string CGROUP_PATH = "/sys/fs/cgroup/minicontainer";

        bool apply_limits(pid_t child_pid, const std::string& mem_limit_bytes) {
            std::cout << "[Cgroup] Provisioning resource sandbox directory at: " << CGROUP_PATH << "\n";

            if (mkdir(CGROUP_PATH.c_str(), 0755) == -1 && errno != EEXIST) {
                std::cerr << "[Cgroup] Error: Failed to create sandbox folder structure.\n";
                return false;
            }

            
            std::ofstream mem_max_file(CGROUP_PATH + "/memory.max");
            if (!mem_max_file.is_open()) {
                std::cerr << "[Cgroup] Error: Cannot configure memory limits. Ensure cgroups v2 is active.\n";
                return false;
            }
            mem_max_file << mem_limit_bytes;
            mem_max_file.close();

            
            std::ofstream procs_file(CGROUP_PATH + "/cgroup.procs");
            if(!procs_file.is_open()){
                std::cerr << "[Cgroup] Error: Failed to register container PID boundary.\n";
                return false;
            }
            procs_file << child_pid;
            procs_file.close();

            std::cout << "[Cgroup] Resource rules applied. Memory constrained at: " << mem_limit_bytes << " bytes.\n";
            return true;
        }

        void cleanup(){
            std::cout<< "[Cgroup] Sweeping resource sandbox runtime structures...\n";

            rmdir(CGROUP_PATH.c_str());
        }
    }
}