#!/bin/bash

set -e

# Extract version from CMakeLists.txt LIBM2K_VERSION_MAJOR/MINOR/PATCH variables for the CI_VERSION
cmake_version=$(grep -oP 'set\s*\(\s*LIBM2K_VERSION_(MAJOR|MINOR|PATCH)\s+\K[0-9]+' CMakeLists.txt | paste -sd '.')

if [[ -z "$cmake_version" ]]; then
	echo "Failed to extract version from CMakeLists.txt, using default"
	cmake_version="0.0.1+${GITHUB_SHA}"
fi

echo "CI Version to use: $cmake_version"
echo "ci-version=$cmake_version" >> "$GITHUB_OUTPUT"

# Extract version from ref for the CLOUDSMITH_VERSION
if [[ "${GITHUB_REF}" == *"refs/tags/v"* ]]; then
	cloudsmith_version="libm2k-${GITHUB_REF#refs/tags}"
elif [[ "${GITHUB_REF}" == "refs/heads/main" ]]; then
	cloudsmith_version="libm2k-main~latest"
elif [[ "${GITHUB_REF}" == *"refs/pull/"* ]]; then
	pr_number=$(echo "$GITHUB_REF" | sed -e "s#refs/##g" -e "s#/merge##g" -e "s#/#-#g")
	cloudsmith_version="libm2k-${pr_number}"
fi

echo "Cloudsmith version to use: $cloudsmith_version"
echo "cloudsmith-version=$cloudsmith_version" >> "${GITHUB_OUTPUT}"
