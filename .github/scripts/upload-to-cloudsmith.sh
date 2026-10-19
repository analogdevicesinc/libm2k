#!/bin/bash

set -e

ARTIFACTS=$(find "${WORKSPACE}/artifacts" -type f -not -name "*.orig.tar.gz*" -not -name "*.debian.tar.xz*" -not -name "*.dsc")

while IFS="" read -r file; do
    filename=$(basename "${file}")
    [[ "${filename}" == "Artifact manifest" ]] && continue

    if [[ "${filename}" == "Windows-*" ]]; then
        cd "${ARTIFACTS}"
        zip -r "${filename}.zip" "${filename}"
        rm -rf "${filename}"
        cloudsmith push raw --republish --tags "${filename}" --version "${VERSION}" \
            ${CLOUDSMITH_ORG}/${CLOUDSMITH_REPO} "${filename}.zip"
        rm "${tag}.zip"
        cd -
        continue
    fi

    # Strip git hash (.g<hex>) so the name is stable across commits
    stable=$(echo "${filename}" | sed 's/\.g[0-9a-f]\+//')
    ext="${stable##*.}"
    stable="${stable%.${ext}}"
    cloudsmith push raw --republish --name "${stable}.${ext}" --tags "${stable}" --version "${VERSION}" \
        ${CLOUDSMITH_ORG}/${CLOUDSMITH_REPO} "${file}"
done <<< "$ARTIFACTS"
