#!/bin/bash 

make
arm-linux-gcc -o inter_syn_test inter_test.c
cp inter_botton_syn.ko inter_syn_test ../../linuxkernel/myfs1/ 
