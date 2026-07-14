#ifndef LIFECYCLE_H
#define LIFECYCLE_H

#include "container_config.h"

namespace Core {
    void start_container(const ContainerArgs& args);

    int container_main(void* args);
}

#endif 