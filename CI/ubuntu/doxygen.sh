#!/bin/bash

TOP_DIR=$(pwd)

echo_red() { printf "\033[1;31m$*\033[m\n"; }
echo_green() { printf "\033[1;32m$*\033[m\n"; }

############################################################################
# Check if the documentation will be generated w/o warnings or errors
############################################################################
pushd ${TOP_DIR}/build
(cd doc && ! make doc 2>&1 | grep -E "warning|error") || {
        echo_red "Documentation incomplete or errors in the generation of it have occured!"
        exit 1
}

echo_green "Documentation was generated successfully!"

############################################################################
# If the current build is not a pull request and it is on the default branch the
# documentation will be pushed to the gh-pages branch
############################################################################
if [[ "${IS_PULL_REQUEST}" == "False" && "${BRANCH_NAME}" == "main" ]]
then
        echo_green "Running Github docs update on commit '$CURRENT_COMMIT'"
        git config --global user.email "cse-ci-notifications@analog.com"
        git config --global user.name "CSE-CI"

        pushd ${TOP_DIR}
        git fetch --depth 1 origin +refs/heads/gh-pages:gh-pages
        git checkout gh-pages
        mkdir -p ${TOP_DIR}/python

        cp -R ${TOP_DIR}/build/doc/doxygen_doc/html/* ${TOP_DIR}
        cp -R ${TOP_DIR}/build/doc/doxygen_doc/python/* ${TOP_DIR}/python

        sudo rm -rf ${TOP_DIR}/doc
        sudo rm -rf ${TOP_DIR}/glog
        sudo rm -rf ${TOP_DIR}/build

	# Need to create a .nojekyll file to allow filenames starting with an underscore
	# to be seen on the gh-pages site. Therefore creating an empty .nojekyll file.
	if [ ! -f ".nojekyll" ] ; then
		touch .nojekyll
	fi

        GH_CURRENT_COMMIT=$(git log -1 --pretty=%B)
        if [[ ${GH_CURRENT_COMMIT:(-7)} != ${CURRENT_COMMIT:0:7} ]]
        then
                git add --all .
                git commit --allow-empty --amend -m "Update documentation to ${CURRENT_COMMIT:0:7}"
                if [ -n "$GITHUB_DOC_TOKEN" ] ; then
                        git push https://${GITHUB_DOC_TOKEN}@github.com/${REPO_SLUG} gh-pages -f
                else
                        git push origin gh-pages -f
                fi
                echo_green "Documentation updated!"
        else
                echo_green "Documentation already up to date!"
        fi
else
        echo_green "Documentation will be updated when this commit gets on the default branch!"
fi
