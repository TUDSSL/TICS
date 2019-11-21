#!/bin/bash

dirname="msp430-gcc-7.3.1.24-source-full"
filename="$dirname.tar.bz2"
download_url="http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/6_0_1_0/exports/msp430-gcc-7.3.1.24-source-full.tar.bz2"

if [ -d $dirname/gcc ]; then
    if [ $(ls $dirname -1 | wc -l) -gt 1 ]; then
        echo "More than gcc directory in $dirname"
	echo "This will extract the origional files to $dirname"
	echo "And will remove changed files not in the gcc directory"
	read -p "Would you like to overwrite the directory (y/N)? " -n 1 -r
	if [[ $REPLY =~ ^[Yy]$ ]]; then
		"Overwriting files..."
	else
		echo ""
		exit 1
	fi
    fi
fi

if [ ! -f $filename ]; then
    echo "Downloading $filename"
    wget "$download_url"
fi

echo "Extracting $filename"
tar xvjf $filename --exclude=gcc
echo "Done!" && \
exit 1

echo "Something went wrong"
exit 0
