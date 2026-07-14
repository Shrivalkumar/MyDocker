#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <sys/types.h>

namespace Core{
    namespace Network{

        bool setup_network(pid_t child_pid);

        void cleanup_network();
    }
}

#endif