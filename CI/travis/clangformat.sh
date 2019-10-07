#!/bin/bash

echo_red() { printf "\033[1;31m$*\033[m\n"; }
echo_green() { printf "\033[1;32m$*\033[m\n"; }

############################################################################
# Check if the file given as input has .h or .cpp extension
############################################################################
is_source_file() {
    local file="$1"

    EXTENSIONS=".hpp .cpp"

    for extension in $EXTENSIONS; do
        [[ "${file: -2}" == "$extension" || "${file: -4}" == "$extension" ]] && return 0
    done;

    return 1
}


############################################################################
# Check if the files modified in the current commit / commit range respect
# the coding style defined in the .clang-format file
############################################################################
COMMIT_RANGE=$TRAVIS_COMMIT_RANGE

if [ -z "$TRAVIS_PULL_REQUEST_SHA" ]
then
    COMMIT_RANGE=HEAD~1
fi

git diff --name-only --diff-filter=d $COMMIT_RANGE | while read -r file; do
        if is_source_file "$file"
        then
            /usr/bin/clang-format-6.0 -i "$file"
        fi
done;

git diff --exit-code || {
    echo_red "The code is not properly formatted!"
    exit 1
}

echo_green "The code is properly formatted!"
