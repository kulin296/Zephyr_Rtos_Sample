# Zephyr development image for Venus Project

# Settings
ARG DEBIAN_VERSION=stable-20241016-slim
ARG PASSWORD="zephyr"
ARG ZEPHYR_RTOS_VERSION=4.1.0
ARG ZEPHYR_RTOS_COMMIT=7823374e872145b5bd018bfe447839eb36042611
ARG ZEPHYR_SDK_VERSION=0.16.8
ARG VS_CODE_SERVER_VERSION=4.93.1
ARG VS_CODE_SERVER_PORT=8800
ARG VS_CODE_EXT_CPPTOOLS_VERSION=1.22.10
ARG VS_CODE_EXT_HEX_EDITOR_VERSION=1.11.1
ARG VS_CODE_EXT_CMAKETOOLS_VERSION=1.19.52
ARG VS_CODE_EXT_NRF_DEVICETREE_VERSION=2024.9.26
ARG VS_CODE_EXT_CORTEX_DEBUG_VERSION=1.12.1
ARG VS_CODE_EXT_DEBUG_TRACKER_VERSION=0.0.15
ARG VS_CODE_EXT_MEMORY_VIEW_VERSION=0.0.25
ARG VS_CODE_EXT_RTOS_VIEWS_VERSION=0.0.7
ARG VS_CODE_EXT_PERIPHERAL_VIEWER_VERSION=1.4.6
ARG TOOLCHAIN_LIST="-t x86_64-zephyr-elf -t arm-zephyr-eabi"
ARG WGET_ARGS="-q --show-progress --progress=bar:force:noscroll"
ARG VIRTUAL_ENV=/opt/venv

#-------------------------------------------------------------------------------
# Base Image and Dependencies

# Use Debian as the base image
FROM debian:${DEBIAN_VERSION}

# Redeclare arguments after FROM
ARG PASSWORD
ARG ZEPHYR_RTOS_VERSION
ARG ZEPHYR_RTOS_COMMIT
ARG ZEPHYR_SDK_VERSION
ARG VS_CODE_SERVER_VERSION
ARG VS_CODE_SERVER_PORT
ARG VS_CODE_EXT_CPPTOOLS_VERSION
ARG VS_CODE_EXT_HEX_EDITOR_VERSION
ARG VS_CODE_EXT_CMAKETOOLS_VERSION
ARG VS_CODE_EXT_NRF_DEVICETREE_VERSION
ARG VS_CODE_EXT_CORTEX_DEBUG_VERSION
ARG VS_CODE_EXT_DEBUG_TRACKER_VERSION
ARG VS_CODE_EXT_MEMORY_VIEW_VERSION
ARG VS_CODE_EXT_RTOS_VIEWS_VERSION
ARG VS_CODE_EXT_PERIPHERAL_VIEWER_VERSION
ARG TOOLCHAIN_LIST
ARG WGET_ARGS
ARG VIRTUAL_ENV
ARG TARGETARCH

# Set default shell during Docker image build to bash
SHELL ["/bin/bash", "-c"]

# Check if the target architecture is either x86_64 (amd64) or arm64 (aarch64)
RUN if [ "$TARGETARCH" = "amd64" ] || [ "$TARGETARCH" = "arm64" ]; then \
        echo "Architecture $TARGETARCH is supported."; \
    else \
        echo "Unsupported architecture: $TARGETARCH"; \
        exit 1; \
    fi

# Set non-interactive frontend for apt-get to skip any user confirmations
ENV DEBIAN_FRONTEND=noninteractive

# Install base packages
RUN apt-get -y update && \
    apt-get install --no-install-recommends -y \
        ca-certificates \
        file \
        locales \
        git \
        build-essential \
        cmake \
        ninja-build gperf \
        device-tree-compiler \
        wget \
        curl \
        python3 \
        python3-pip \
        python3-venv \
        xz-utils \
        dos2unix \
        vim \
        nano \
        mc \
        gdb \
        iputils-ping \
        openssh-server

# Set root password
RUN echo "root:${PASSWORD}" | chpasswd

# Set up a Python virtual environment
ENV VIRTUAL_ENV=${VIRTUAL_ENV}
RUN python3 -m venv ${VIRTUAL_ENV}
ENV PATH="${VIRTUAL_ENV}/bin:$PATH"

# Install west
RUN python3 -m pip install --no-cache-dir west

# Clean up stale packages
RUN apt-get clean -y && \
    apt-get autoremove --purge -y && \
    rm -rf /var/lib/apt/lists/*

# Set up directories
RUN mkdir -p /workspace/ && \
    mkdir -p /opt/toolchains

# Set up sshd working directory
RUN mkdir -p /var/run/sshd && \
    chmod 0755 /var/run/sshd

# Allow root login via SSH
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# Expose SSH port
EXPOSE 22

#-------------------------------------------------------------------------------
# Zephyr RTOS

# Set Zephyr environment variables
ENV ZEPHYR_RTOS_VERSION=${ZEPHYR_RTOS_VERSION}

# Install Zephyr
RUN cd /opt/toolchains && \
    git clone https://github.com/zephyrproject-rtos/zephyr.git && \
    cd zephyr && \
    git checkout ${ZEPHYR_RTOS_COMMIT} && \
    python3 -m pip install -r scripts/requirements-base.txt

# Override the west manifest to only install necessary modules
COPY scripts/st/west.yml /opt/toolchains/zephyr/west.yml

# Instantiate west workspace and install tools
RUN cd /opt/toolchains && \
    west init -l zephyr && \
    west update --narrow -o=--depth=1

#-------------------------------------------------------------------------------
# Zephyr SDK

# Set environment variables
ENV ZEPHYR_SDK_VERSION=${ZEPHYR_SDK_VERSION}

# Install minimal Zephyr SDK
RUN cd /opt/toolchains && \
    wget ${WGET_ARGS} https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-${HOSTTYPE}_minimal.tar.xz && \
    tar xf zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-${HOSTTYPE}_minimal.tar.xz && \
    rm zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-${HOSTTYPE}_minimal.tar.xz

# Install Zephyr SDK
RUN cd /opt/toolchains/zephyr-sdk-${ZEPHYR_SDK_VERSION} && \
    bash setup.sh -c ${TOOLCHAIN_LIST}

# Install host tools
RUN cd /opt/toolchains/zephyr-sdk-${ZEPHYR_SDK_VERSION} && \
    wget ${WGET_ARGS} https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/hosttools_linux-${HOSTTYPE}.tar.xz && \
    tar xf hosttools_linux-${HOSTTYPE}.tar.xz && \
    rm hosttools_linux-${HOSTTYPE}.tar.xz && \
    bash zephyr-sdk-${HOSTTYPE}-hosttools-standalone-*.sh -y -d .

#-------------------------------------------------------------------------------
# VS Code Server

# Set VS Code Server environment variables
ENV VS_CODE_SERVER_VERSION=${VS_CODE_SERVER_VERSION}
ENV VS_CODE_SERVER_PORT=${VS_CODE_SERVER_PORT}

# Install VS Code Server
RUN cd /tmp && \
    wget ${WGET_ARGS} --no-check-certificate https://code-server.dev/install.sh && \
    chmod +x install.sh && \
    bash install.sh --version ${VS_CODE_SERVER_VERSION}

# Download VS Code extensions (code-server extension manager does not work well)
RUN cd /tmp && \
    if [ "$TARGETARCH" = "amd64" ]; then \
        wget ${WGET_ARGS} https://github.com/microsoft/vscode-cpptools/releases/download/v${VS_CODE_EXT_CPPTOOLS_VERSION}/cpptools-linux-x64.vsix -O cpptools.vsix; \
    elif [ "$TARGETARCH" = "arm64" ]; then \
        wget ${WGET_ARGS} https://github.com/microsoft/vscode-cpptools/releases/download/v${VS_CODE_EXT_CPPTOOLS_VERSION}/cpptools-linux-arm64.vsix -O cpptools.vsix; \
    else \
        echo "Unsupported architecture"; \
        exit 1; \
    fi && \
    wget ${WGET_ARGS} https://github.com/microsoft/vscode-cmake-tools/releases/download/v${VS_CODE_EXT_CMAKETOOLS_VERSION}/cmake-tools.vsix -O cmake-tools.vsix && \
    wget --compression=gzip ${WGET_ARGS} https://marketplace.visualstudio.com/_apis/public/gallery/publishers/ms-vscode/vsextensions/hexeditor/${VS_CODE_EXT_HEX_EDITOR_VERSION}/vspackage -O ms-vscode-hexeditor-${VS_CODE_EXT_HEX_EDITOR_VERSION}.vsix && \
    wget --compression=gzip https://marketplace.visualstudio.com/_apis/public/gallery/publishers/nordic-semiconductor/vsextensions/nrf-devicetree/${VS_CODE_EXT_NRF_DEVICETREE_VERSION}/vspackage -O nordic-semiconductor.nrf-devicetree-${VS_CODE_EXT_NRF_DEVICETREE_VERSION}.vsix && \
    wget --compression=gzip https://marketplace.visualstudio.com/_apis/public/gallery/publishers/marus25/vsextensions/cortex-debug/${VS_CODE_EXT_CORTEX_DEBUG_VERSION}/vspackage -O marus25.cortex-debug-${VS_CODE_EXT_CORTEX_DEBUG_VERSION}.vsix && \
    wget --compression=gzip https://marketplace.visualstudio.com/_apis/public/gallery/publishers/mcu-debug/vsextensions/debug-tracker-vscode/${VS_CODE_EXT_DEBUG_TRACKER_VERSION}/vspackage -O mcu-debug.debug-tracker-vscode-${VS_CODE_EXT_DEBUG_TRACKER_VERSION}.vsix && \
    wget --compression=gzip https://marketplace.visualstudio.com/_apis/public/gallery/publishers/mcu-debug/vsextensions/memory-view/${VS_CODE_EXT_MEMORY_VIEW_VERSION}/vspackage -O mcu-debug.memory-view-${VS_CODE_EXT_MEMORY_VIEW_VERSION}.vsix && \
    wget --compression=gzip https://marketplace.visualstudio.com/_apis/public/gallery/publishers/mcu-debug/vsextensions/peripheral-viewer/${VS_CODE_EXT_PERIPHERAL_VIEWER_VERSION}/vspackage -O mcu-debug.peripheral-viewer-${VS_CODE_EXT_PERIPHERAL_VIEWER_VERSION}.vsix && \
    wget --compression=gzip https://marketplace.visualstudio.com/_apis/public/gallery/publishers/mcu-debug/vsextensions/rtos-views/${VS_CODE_EXT_RTOS_VIEWS_VERSION}/vspackage -O mcu-debug.rtos-views-${VS_CODE_EXT_RTOS_VIEWS_VERSION}.vsix

# Install extensions
RUN cd /tmp && \
    code-server --install-extension cpptools.vsix && \
    code-server --install-extension cmake-tools.vsix && \
    code-server --install-extension ms-vscode-hexeditor-${VS_CODE_EXT_HEX_EDITOR_VERSION}.vsix && \
    code-server --install-extension nordic-semiconductor.nrf-devicetree-${VS_CODE_EXT_NRF_DEVICETREE_VERSION}.vsix && \
    code-server --install-extension mcu-debug.debug-tracker-vscode-${VS_CODE_EXT_DEBUG_TRACKER_VERSION}.vsix && \
    code-server --install-extension mcu-debug.memory-view-${VS_CODE_EXT_MEMORY_VIEW_VERSION}.vsix && \
    code-server --install-extension mcu-debug.rtos-views-${VS_CODE_EXT_RTOS_VIEWS_VERSION}.vsix && \
    code-server --install-extension mcu-debug.peripheral-viewer-${VS_CODE_EXT_PERIPHERAL_VIEWER_VERSION}.vsix && \
    code-server --install-extension marus25.cortex-debug-${VS_CODE_EXT_CORTEX_DEBUG_VERSION}.vsix

# Copy workspace configuration
COPY scripts/zephyr.code-workspace /zephyr.code-workspace

#-------------------------------------------------------------------------------
# Optional Settings

# Initialise system locale (required by menuconfig)
RUN sed -i '/^#.*en_US.UTF-8/s/^#//' /etc/locale.gen && \
    locale-gen en_US.UTF-8 && \
    update-locale LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8

# Use the "dark" theme for Midnight Commander
ENV MC_SKIN=dark

#-------------------------------------------------------------------------------
# Entrypoint

# Activate the Python and Zephyr environments for shell sessions
RUN echo "source ${VIRTUAL_ENV}/bin/activate" >> /root/.bashrc && \
    echo "source /opt/toolchains/zephyr/zephyr-env.sh" >> /root/.bashrc

# entrypoint
COPY scripts/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
