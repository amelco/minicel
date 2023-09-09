#!/bin/bash

set -xe

gcc -Wall -Wextra -ggdb --std=c11 sheet.h globals.h mystr.h main.c -o minicel
