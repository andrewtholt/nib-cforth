#!/bin/bash

# set -x

ECHO=$(which echo)

printf "\n"
printf "// Autogenerated, do not edit\n\n"
printf "#ifndef __ATL_SRC\n"
printf "#define __ATL_SRC\n\n"

printf "uint8_t nvramrc[] = \""

while read -r LINE; do 
    if [ ${#LINE} -ge 0 ]; then
        T=$($ECHO $LINE | grep -o '^[^\\]*') 
        L=$($ECHO $T | grep -o '^[^(]*')
        M=$($ECHO $L | sed 's/\"/\\\"/g')

        if [  ${#M} -gt 0 ]; then
            $ECHO -n "$M\\n"
        fi
#        if [ "$L" = " \\" ]; then
#            exit
#        else
#            $ECHO -n "$L \\" # | sed '/^\s*$/d'
#            printf "n"
#        fi
    fi
done 
printf "\";\n\n"

printf "#endif\n\n"
