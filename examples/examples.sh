#!/bin/bash

make clean && gcc xdbg.c examples.c -o examples && ./examples
