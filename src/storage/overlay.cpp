#include "storage/overlay.h"
#include <iostream>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

namespace Storage {
    bool jail_filesystem(const std::string& rootfs_path) {

        std::string container_dev = rootfs_path + "/dev";
        if(mount("/dev", container_dev.c_str(), nullptr, MS_BIND | MS_REC, nullptr) < 0){
            std::cerr << "[Storage] Failed to bind mount /dev: " << std::strerror(errno) << "\n";
            return false;
        }


        if(chroot(rootfs_path.c_str()) == -1){
            std::cerr << "[Storage] Critical: chroot jail execution failed.\n";
            return false;
        }


        if(chdir("/") == -1){
            return false;
        }


        if(mount("proc", "/proc", "proc", 0, nullptr) < 0){
            std::cerr << "[Storage] Proc allocation mapping failed inside jail.\n";
            return false;
        }

        return true;
    }
}