#ifndef OVERLAY_H
#define OVERLAY_H

#include <string>

namespace Storage {
    // Alters process filesystem root path and initializes isolated virtual kernel mount structures
    bool jail_filesystem(const std::string& rootfs_path);
}

#endif