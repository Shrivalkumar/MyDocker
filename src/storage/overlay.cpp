#include "storage/overlay.h"
#include <iostream>
#include <unistd.h>
#include <sys/mount.h>
#include <cstring>
#include <cerrno>

namespace Storage {

    bool jail_filesystem(const std::string& rootfs_path) {
        std::cout << "[Storage] Engaging chroot boundary jail at: " << rootfs_path << "\n";

        // 1. Pivot process view of system folder tree down to the image target directory
        if (chroot(rootfs_path.c_str()) == -1) {
            std::cerr << "[Storage] System execution error: chroot configuration rejected: " 
                      << std::strerror(errno) << "\n";
            return false;
        }

        // 2. Pivot internal working directory pointer to new relative system root
        if (chdir("/") == -1) {
            std::cerr << "[Storage] System error: Failed to drop down to new directory root: " 
                      << std::strerror(errno) << "\n";
            return false;
        }

        std::cout << "[Storage] Isolating dynamic kernel process descriptors via /proc mount...\n";

        // 3. Mount namespace-isolated proc instance so process tracking handles cleanly
        if (mount("proc", "/proc", "proc", 0, nullptr) == -1) {
            std::cerr << "[Storage] Critical: Failed to link local /proc boundary: " 
                      << std::strerror(errno) << "\n";
            return false;
        }

        return true;
    }
}