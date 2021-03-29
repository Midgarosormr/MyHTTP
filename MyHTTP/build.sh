#!/bin/bash

root=/var/MyHTTP/html/;

if [ ! -d "${root}" ];then
	mkdir ${root}
fi
if [ ! -d "./bin/" ];then
	mkdir bin/
fi

make all
