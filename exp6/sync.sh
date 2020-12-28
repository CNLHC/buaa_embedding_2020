#!/bin/env bash

cd cmake-build/
make all

scp ./clock embed:~/fjhc
scp ./led_hex embed:~/fjhc
scp ./myzr_zlg7290.ko embed:~/fjhc
cd .. 

