#!/bin/bash

set -e

# Extract version from CMakeLists.txt LIBM2K_VERSION_MAJOR/MINOR/PATCH variables
cmake_version=$(grep -oP 'set\s*\(\s*LIBM2K_VERSION_(MAJOR|MINOR|PATCH)\s+\K[0-9]+' CMakeLists.txt | paste -sd '.')

if [[ -z "$cmake_version" ]]; then
	echo "Failed to extract version from CMakeLists.txt, using default"
	cmake_version="0.0.1+${GITHUB_SHA}"
fi

echo "Version to use: $cmake_version"
echo "version=$cmake_version" >> "$GITHUB_OUTPUT"

