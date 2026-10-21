#!/bin/bash

set -e

[[ "$ARCHITECTURE" == "arm64" ]] && SUDO="" || SUDO="sudo"

# Manual docker run for amrhf architecture
if [[ "$ARCHITECTURE" == "armhf" ]]; then
    # Start container
    docker run --platform ${PLATFORM} \
        --name debian13-armhf \
        -v "${GITHUB_WORKSPACE}:/workspace/libm2k" \
        -dit ${CONTAINER}
    
    # Install deps
    echo "::group::Installing common dependencies"
    docker exec \
        debian13-armhf \
        /bin/bash -c "
            set -e
            cd /workspace/libm2k
            apt-get update && apt-get install -y \
            rpm pybuild-plugin-pyproject dh-python tar doxygen libgoogle-glog-dev \
            libc6 libxml2-dev libcdk5-dev libaio-dev libusb-1.0-0-dev libserialport-dev \
            libavahi-client-dev bison flex wget graphviz libavahi-common-dev bzip2 curl
            (apt-get install -y policykit-1 || apt-get install -y polkitd pkexec) && \
            apt-get install -y python3-gi-cairo swig
        "
    echo "::endgroup::"

    # Install libiio
    echo "::group::Installing libiio"
    if [[ "$STAGE" == "dev" ]]; then
        docker exec \
            -e LIBIIO_ARTIFACT="${LIBIIO_ARTIFACT}" \
            debian13-armhf \
            /bin/bash -c "
                wget https://packages.analog.com/public/raw/versions/libiio-v0~latest/libiio-0.26.g-${LIBIIO_ARTIFACT}
                dpkg -i *.deb
                ldconfig
                apt-get install -y python3-libiio
            "
    else
        docker exec \
            debian13-armhf \
            /bin/bash -c "
                apt-get install -y libiio-dev
                ldconfig
                apt-get install -y python3-libiio
            "
    fi
    echo "::endgroup::"
else
    echo "::group::Installing common dependencies"
    ${SUDO} apt-get update
    DEBIAN_FRONTEND=noninteractive ${SUDO} apt-get install -y \
            rpm pybuild-plugin-pyproject dh-python tar doxygen libgoogle-glog-dev \
            libc6 libxml2-dev libcdk5-dev libaio-dev libusb-1.0-0-dev libserialport-dev \
            libavahi-client-dev bison flex wget graphviz libavahi-common-dev bzip2 curl

    (${SUDO} apt-get install -y policykit-1 || ${SUDO} apt-get install -y polkitd pkexec) && \
        ${SUDO} apt-get install python3-gi-cairo
    python3 -m pip install requests
    echo "::endgroup::"

    # Install libiio
    echo "::group::Installing libiio"
    if [[ "$STAGE" == "dev" ]]; then
        wget https://packages.analog.com/public/raw/versions/libiio-v0~latest/libiio-0.26.g-${LIBIIO_ARTIFACT}
          
        ${SUDO} dpkg -i *.deb
        ${SUDO} ldconfig
    else
        ${SUDO} apt-get install -y libiio-dev
        ${SUDO} ldconfig
    fi

    # The libiio comes by default with the package, but for checking
    ${SUDO} apt-get install -y python3-libiio
    echo "::endgroup::"
fi
