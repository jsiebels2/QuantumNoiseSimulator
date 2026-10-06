FROM mcr.microsoft.com/devcontainers/base:ubuntu-24.04

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive && apt-get install -y \
      build-essential cmake ninja-build gdb \
      clang clangd clang-format \
      libeigen3-dev libgtest-dev \
      python3 python3-dev python3-venv \
    && rm -rf /var/lib/apt/lists/*