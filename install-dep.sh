#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    if [[ $EUID -ne 0 ]]; then
        echo "This script must be run as root."
        exit
    fi

    apt-get install libcurl4-openssl-dev
    apt-get install cmake
elif [[ "$OSTYPE" == "darwin"* ]]; then
    brew install curl-openssl
fi
