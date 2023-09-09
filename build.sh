#!/bin/bash

set -xe

gcc -Wall -Wextra -ggdb --std=c11 globals.h mystr.h main.c -o minicel
