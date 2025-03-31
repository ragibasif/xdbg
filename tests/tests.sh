#!/bin/bash

make clean && gcc xdbg.c tests.c -o tests && ./tests
