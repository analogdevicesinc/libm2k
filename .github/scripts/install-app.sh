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
        -e VERSION_SUFFIX="$VERSION_SUFFIX" \
        -w /workspace/libm2k \
        debian-armhf \
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
        -e VERSION_SUFFIX="$VERSION_SUFFIX" \
        debian-armhf \
        /bin/bash -c "
            set -e
            apt-get install -y /workspace/artifacts/libm2k_${CI_VERSION}-1${VERSION_SUFFIX}_${ARCHITECTURE}.deb
        "
    
    # Copy from docker to host
    mkdir -p artifacts
    docker cp debian-armhf:/workspace/artifacts/. ./artifacts/
    rm -rf artifacts/libm2k_${CI_VERSION}.orig.tar
else
    [[ "$ARCHITECTURE" == "arm64" ]] && SUDO="" || SUDO="sudo"
    if [[ "$STAGE" == "rel" ]]; then
        ${SUDO} apt-get install -y "${ARTIFACTS_DIR}/libm2k_${CI_VERSION}-1${VERSION_SUFFIX}_${ARCHITECTURE}.deb"
    else
        ${SUDO} dpkg -i "${ARTIFACTS_DIR}/libm2k_${CI_VERSION}-1${VERSION_SUFFIX}_${ARCHITECTURE}.deb"
    fi

    rm -rf artifacts/libm2k_${CI_VERSION}.orig.tar
fi

# Create debian and wheels outputs name - differentiate by distro
if [[ "${CONTAINER}" == *"kuiper"* ]]; then
    distro_tag="kuiper"
elif [[ "${CONTAINER}" == "debian:latest" || "${CONTAINER}" == "debian:forky" ]]; then
    distro_tag="debian"
else
    distro_tag="ubuntu"
fi

# On armhf the os-release has to be read from inside the container
if [[ "$ARCHITECTURE" == 'armhf' ]]; then
    os_release=$(docker exec debian-armhf cat /etc/os-release)
else
    os_release=$(cat /etc/os-release)
fi

version_id=$(echo "$os_release" | grep "^VERSION_ID" | cut -d "=" -f2 | tr -d '"' || true)

# Set distribution name based on os release version
if [[ "${VERSION_SUFFIX}" == *"Debian14"* ]]; then
    distribution_name="forky"
else
    distribution_name="trixie"
fi
# Debian testing (forky) has no VERSION_ID - fall back to the codename
[[ -z "$version_id" ]] && version_id=$(echo "$os_release" | grep "^VERSION_CODENAME" | cut -d "=" -f2 | tr -d '"') || true

echo "LIBM2K_DEBIAN=libm2k-deb-${ARCHITECTURE}-${distro_tag}-${version_id}-${STAGE}" >> $GITHUB_ENV
echo "LIBM2K_WHEELS=libm2k-wheels-${ARCHITECTURE}-${distro_tag}-${version_id}-${STAGE}" >> $GITHUB_ENV
echo "DISTRIBUTION_NAME=${distribution_name}" >> "$GITHUB_ENV"
