#!/bin/bash

set -e

# Detect manual container build or default
if [[ "$ARCHITECTURE" == 'armhf' ]]; then
    # Also manual build the debian package for armhf
    echo "Building debian package for armhf ..."
    [[ "$STAGE" == "dev" ]] && control_path="packaging/debian_dev" || control_path="packaging/debian"

    docker exec \
        -e VERSION="$VERSION"\
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
        -e VERSION="$VERSION" \
        debian13-armhf \
        /bin/bash -c "
            set -e
            apt-get install -y /workspace/artifacts/libm2k_${VERSION}-1_${ARCHITECTURE}.deb
        "
    
    # Copy from docker to host
    mkdir -p artifacts
    docker cp debian13-armhf:/workspace/artifacts/. ./artifacts/
else
    [[ "$ARCHITECTURE" == "arm64" ]] && SUDO="" || SUDO="sudo"
    [[ "$STAGE" == "rel" ]] && \
        ${SUDO} apt-get install -y "${ARTIFACTS_DIR}/libm2k_${VERSION}-1_${ARCHITECTURE}.deb" || \
        ${SUDO} dpkg -i "${ARTIFACTS_DIR}/libm2k_${VERSION}-1_${ARCHITECTURE}.deb"
fi
