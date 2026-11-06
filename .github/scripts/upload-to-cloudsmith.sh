#!/bin/bash

set -eo pipefail

# Determine distro tag from container
if [[ "${CONTAINER}" == *"kuiper"* ]]; then
    DISTRO_TAG="-kuiper"
elif [[ "${CONTAINER}" == "debian:latest" ]]; then
    DISTRO_TAG="-debian"
else
    DISTRO_TAG=""
fi

ARTIFACTS=$(find "${WORKSPACE}/artifacts" -type f -not -name "*.orig.tar.gz*" -not -name "*.debian.tar.xz*" -not -name "*.dsc")

while IFS="" read -r file; do
    filename=$(basename "${file}")
    [[ "${filename}" == "Artifact manifest" ]] && continue

    if [[ "${filename}" == "Windows-*" ]]; then
        cd "${ARTIFACTS}"
        zip -r "${filename}.zip" "${filename}"
        rm -rf "${filename}"
        cloudsmith push raw --republish --no-wait-for-sync --tags "${filename}" --version "${CLOUDSMITH_VERSION}" \
            ${CLOUDSMITH_ORG}/${CLOUDSMITH_REPO} "${filename}.zip"
        rm "${tag}.zip"
        cd -
        continue
    fi

    # Strip git hash (.g<hex>) so the name is stable across commits
    stable=$(echo "${filename}" | sed 's/\.g[0-9a-f]\+//')
    ext="${stable##*.}"
    stable="${stable%.${ext}}"
    # Add distro tag to differentiate kuiper vs debian builds
    cloudsmith push raw --republish --no-wait-for-sync --name "${stable}${DISTRO_TAG}.${ext}" --tags "${stable}${DISTRO_TAG}" --version "${CLOUDSMITH_VERSION}" \
        ${CLOUDSMITH_ORG}/${CLOUDSMITH_REPO} "${file}"
done <<< "$ARTIFACTS"
