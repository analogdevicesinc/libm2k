#!/bin/bash

set -eo pipefail

# Detect manual container build or default
if [[ "$ARCHITECTURE" == 'armhf' ]]; then
    # Also manual build the debian package for armhf
    echo "Building debian package for armhf ..."
    [[ "$STAGE" == "dev" ]] && control_path="packaging/debian_dev" || control_path="packaging/debian"

    docker exec \
        -e VERSION="$CI_VERSION"\
        -e WORKING_DIRECTORY="/workspace/libm2k" \
        -e GITHUB_WORKSPACE="/workspace" \
        -e IGNORE_PATH="packaging" \
        -e CONTROL_PATH="$control_path" \
        -w /workspace/libm2k \
        debian13-armhf \
        /bin/bash -c "
            set -e
            git clone https://github.com/analogdevicesinc/shared-actions shared-actions
            ./shared-actions/deb-generator-action/build-deb.sh
        "

    # Installation and verification of package
    docker exec \
        -w /workspace/libm2k \
        -e ARCHITECTURE="$ARCHITECTURE" \
        -e VERSION="$CI_VERSION" \
        debian13-armhf \
        /bin/bash -c "
            set -e
            apt-get install -y /workspace/artifacts/libm2k_${CI_VERSION}-1_${ARCHITECTURE}.deb
        "
    
    # Copy from docker to host
    mkdir -p artifacts
    docker cp debian13-armhf:/workspace/artifacts/. ./artifacts/
    rm -rf artifacts/libm2k_${CI_VERSION}.orig.tar
else
    [[ "$ARCHITECTURE" == "arm64" ]] && SUDO="" || SUDO="sudo"
    if [[ "$STAGE" == "rel" ]]; then
        ${SUDO} apt-get install -y "${ARTIFACTS_DIR}/libm2k_${CI_VERSION}-1_${ARCHITECTURE}.deb"
    else
        ${SUDO} dpkg -i "${ARTIFACTS_DIR}/libm2k_${CI_VERSION}-1_${ARCHITECTURE}.deb"
    fi

    rm -rf artifacts/libm2k_${CI_VERSION}.orig.tar
fi

# Create debian and wheels outputs name - differentiate by distro
if [[ "${CONTAINER}" == *"kuiper"* ]]; then
    distro_tag="-kuiper"
elif [[ "${CONTAINER}" == "debian:latest" ]]; then
    distro_tag="-debian"
else
    distro_tag=""
fi
echo "LIBM2K_DEBIAN=libm2k-debian-${ARCHITECTURE}-${STAGE}${distro_tag}" >> $GITHUB_ENV
echo "LIBM2K_WHEELS=libm2k-wheels-${ARCHITECTURE}-${STAGE}${distro_tag}" >> $GITHUB_ENV
