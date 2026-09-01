#!/bin/bash

export GLUEX_TOP=/sciclone/home/jgiroux/gluex_top
export HALLD_VERSIONS=$GLUEX_TOP/halld_versions

source $GLUEX_TOP/gluex_env_boot.sh

VERSION_XML=$HALLD_VERSIONS/version_7.7.0.xml

source $GLUEX_TOP/gluex_env_local.sh $VERSION_XML

export CCDB_CONNECTION="sqlite:////sciclone/data10/jgiroux/Genesis/ccdb.sqlite"
export RCDB_CONNECTION="sqlite:////sciclone/data10/jgiroux/Genesis/rcdb.sqlite"
export JANA_CALIB_URL=$CCDB_CONNECTION
export JANA_RESOURCE_DIR=$GLUEX_TOP/resources
