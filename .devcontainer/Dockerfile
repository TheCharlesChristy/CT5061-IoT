# Alternative Dockerfile-based approach (optional)
# Use this if you need more customization than the features-based approach

FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

# Install system dependencies
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    # Build essentials
    build-essential \
    gcc \
    g++ \
    gdb \
    cmake \
    make \
    ninja-build \
    clang \
    clang-format \
    clang-tidy \
    # Python
    python3 \
    python3-pip \
    python3-dev \
    # ARM toolchain
    gcc-arm-none-eabi \
    gdb-multiarch \
    binutils-arm-none-eabi \
    libnewlib-arm-none-eabi \
    # AVR toolchain
    avr-libc \
    avrdude \
    gcc-avr \
    # Serial and debugging tools
    minicom \
    screen \
    picocom \
    cu \
    usbutils \
    stlink-tools \
    openocd \
    # Additional utilities
    git \
    wget \
    curl \
    vim \
    nano \
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Install Python packages
RUN pip3 install --upgrade pip setuptools wheel \
    && pip3 install \
    pylint \
    black \
    autopep8 \
    flake8 \
    mypy \
    pytest \
    pytest-cov \
    platformio \
    esptool \
    adafruit-ampy \
    pyserial \
    paho-mqtt \
    flask \
    fastapi \
    uvicorn \
    requests \
    numpy \
    pandas \
    matplotlib \
    scipy

# Set up USB permissions
RUN groupadd -f dialout && groupadd -f plugdev

# Default user
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN usermod -aG dialout,plugdev $USERNAME

WORKDIR /workspace

CMD ["/bin/bash"]
