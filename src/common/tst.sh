#!/bin/bash
grep "create_codeword" *.c | grep -v void | grep -v "\/\/" | sed 's/\"//g' |
    cut -f2 -d'(' | sed 's/,/ /g' |
    cut -f1,2 -d' '
