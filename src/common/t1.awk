#!/bin/bash

awk '
/create_codeword/ { 
    tmp = split($0,token,"\"")

    if (length(token[2]) > 0) {
        tmp1 = split( token[3], second, ",")

        if( length(second[2]) > 0 ) {
            print token[2],second[2]
        }
    }
}
'
