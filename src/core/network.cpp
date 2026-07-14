#include "core/network.h"
#include <iostream>
#include <cstdlib>
#include <string>

namespace Core{
    namespace Network{

        bool setup_network(pid_t child_pid){
            std::cout << "[Network] Weaving virtual ethernet fabric for PID: " << child_pid << "...\n";

            
            std::string create_veth = "ip link add veth_host type veth peer name veth_child";
            if(std::system(create_veth.c_str()) != 0){
                std::cerr << "[Network] Failed to allocate virtual device interface.\n";
                return false;
            }

   
            std::system("ip addr add 10.0.0.1/24 dev veth_host");
            std::system("ip link set veth_host up");


            std::string move_to_ns = "ip link set veth_child netns " + std::to_string(child_pid);
            if (std::system(move_to_ns.c_str()) != 0) {
                std::cerr << "[Network] Handshake failed: Interface rejection moving to namespace.\n";
                return false;
            }


            std::string ns_prefix = "nsenter -t " + std::to_string(child_pid) + " -n ";
            std::system((ns_prefix + "ip link set veth_child name eth0").c_str());
            std::system((ns_prefix + "ip addr add 10.0.0.2/24 dev eth0").c_str());
            std::system((ns_prefix + "ip link set eth0 up").c_str());
            std::system((ns_prefix + "ip link set lo up").c_str()); 

            std::cout << "[Network] Network bridge established. Host: 10.0.0.1 <--> Container: 10.0.0.2\n";
            return true;
        }

        void cleanup_network(){
            std::cout << "[Network] Dismantling virtual network cables...\n";
            // deleting the host side which will delete the child side as well 
            std::system("ip link del veth_host 2>/dev/null");
        }
    }
}