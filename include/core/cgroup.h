#ifndef CGROUP_H
#define CGROUP_H

#include <string>
#include <sys/types.h>

namespace Core{
    namespace Cgroup{
        bool apply_limits(pid_t child_pid, const std::string& mem_limit_bytes);

        void cleanup();
    }
}

#endif