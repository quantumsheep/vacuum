#!/bin/bash

if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root."
    exit
fi

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    apt-get install libcurl4-openssl-dev
    apt-get install cmake
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # brew install curl-openssl
fi
