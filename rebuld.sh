#!/bin/sh

make clean
make
sudo rmmod calcul
sudo insmod calcul.ko