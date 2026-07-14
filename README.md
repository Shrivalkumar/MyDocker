# MyDocker

A lightweight Linux containerization engine implemented from scratch in C++. This project demonstrates the fundamental building blocks of modern container runtimes (like Docker), leveraging native Linux kernel abstractions to achieve process isolation, resource control, sandboxed filesystems, and host-network namespace integration.

## 🚀 Key Features

* **Process Isolation (Namespaces):** Leverages the Linux `clone()` system call with specific namespace flags (`CLONE_NEWPID`, `CLONE_NEWUTS`, `CLONE_NEWNS`) to decouple the guest container's process tree, hostname, and mount environment from the host system.
* **Filesystem Sandboxing (Chroot Jail):** Employs `chroot` boundaries alongside dynamic kernel interface virtualization (`mount` on `/proc` and a rec-bind on `/dev`) to guarantee absolute guest-isolation.
* **Parent-Child Synchronization Pipe:** Resolves transient process race conditions for rapid-execution tasks using unbuffered Linux file-descriptor pipes (`pipe()`), ensuring infrastructure setup completes before guest payload execution.
* **Host Network Sharing:** Bridges network barriers seamlessly to host nested microservices (like React/Vite web apps), binding network ports natively to accessible host-loopback endpoints.
* **Resource Constraints Sandbox (Cgroups Infrastructure ready):** Integrated architecture for programmatic resource containment via the Linux Cgroups v2 layout structure.

---

## 🏗️ System Architecture & Mechanics

The runtime lifecycle coordinates execution flow across two distinct operating barriers:

1. **The Parent Monitor:** Allocates process stacks, executes `clone()` constraints, hooks up system dependencies (like virtual device boundaries), handles runtime block loops, and tears down configuration infrastructure upon termination.
2. **The Child Execution Vector:** Intercepts blocking pipelines, resets localized system states (hostname, jail mappings), dynamically mounts proc subsystems, and yields execution context directly over to targeted guest binaries via `execvp`.

---

## 📂 Project Structure

```text
minicontainer/
├── CMakeLists.txt              # Build configuration template
├── include/
│   ├── container_config.h      # Shared runtime configurations & arguments
│   ├── core/
│   │   ├── cgroup.h            # Resource constraint interfaces
│   │   ├── lifecycle.h         # Container boot & sequence orchestrator
│   │   └── network.h           # Host network layout hook routines
│   └── storage/
│       └── overlay.h           # Chroot jailing & bind-mount controls
└── src/
    ├── main.cpp                # Application entrypoint & CLI argument parser
    ├── core/
    │   ├── cgroup.cpp          # Cgroups management implementation
    │   ├── lifecycle.cpp       # Sync mechanics and clone orchestration
    │   └── network.cpp         # Network namespace bindings & cleanup
    └── storage/
        └── overlay.cpp         # Low-level system-call mount layers (/proc, /dev)
```

## 🛠️ Prerequisites & Setup

This application interfaces directly with low-level Linux kernel primitives and must be executed in a standard Linux environment (Ubuntu/Debian or a specialized VS Code Devcontainer runtime shell).

### 1. Install Build Tools

Ensure you have a C++ compiler supporting standard compilation formats (clang++ or g++) along with cmake and ninja:

```bash
sudo apt-get update && sudo apt-get install -y cmake ninja-build build-essential
```

### 2. Download Guest Operating System Base Image

The engine runs commands inside a miniature root filesystem (rootfs). Download a lightweight Alpine Linux distribution rootfs into the target image repository location:

```bash
# Create target image directories
mkdir -p images/alpine

# Download and extract the base distribution layer
wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/alpine-minirootfs-3.19.1-x86_64.tar.gz
tar -xf alpine-minirootfs-3.19.1-x86_64.tar.gz -C images/alpine
rm alpine-minirootfs-3.19.1-x86_64.tar.gz
```

## 🚀 Building and Running

### 🔩 1. Compile the Project

Generate build blueprints using CMake and compile the engine binary:

```bash
cmake -B build -G Ninja
cmake --build build
```

### 🐚 2. Launch an Interactive Guest Shell

To boot into an isolated, interactive Alpine terminal, run the engine without explicit target parameters (defaults to `/bin/sh`):

```bash
sudo ./build/minicontainer
```

Inside the container prompt (`/ #`), you can install packages natively via Alpine's package manager `apk`:

```bash
apk update && apk add git nodejs npm
```

### ⚡ 3. Execute Short-Lived Command Payloads

You can instruct the container engine to evaluate precise string arguments on the fly and exit cleanly:

```bash
sudo ./build/minicontainer echo "Testing isolated runtime vector"
```

## 🌐 Testing Nested Web Apps (e.g., React/Vite Frontend)

Since the engine integrates directly with host network routing interfaces, you can easily spin up complex frontend applications inside the container sandbox and preview them locally.

Boot into the container shell:

```bash
sudo ./build/minicontainer
```

Clone your repository & install dependencies:

```bash
cd /home
git clone https://github.com/Shrivalkumar/Vedantu-Product-Prototype
cd Vedantu-Product-Prototype
npm install
```

Expose the dev server over network boundaries:

```bash
npm run dev -- --host
```

Access the application: Open your host system's web browser and navigate directly to [http://localhost:5173](http://localhost:5173) to test the sandbox container routing pipeline!

## 📜 License

This software is provided under standard educational licensing parameters. Feel free to modify, expand, or fork the logic frameworks to learn more about systems-level software development.
