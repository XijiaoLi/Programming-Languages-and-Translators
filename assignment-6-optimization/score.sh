#!/bin/bash
set -e
shopt -s nullglob

USAGE="Usage:
    $(basename -- "$0") <repo>
"

REPO="$1"
[ ! -d "$REPO" ] && echo "$USAGE" && exit 1

printf "%-20s" "$REPO"

DF_CORR=0 DF_INCO=0 REMOVEDF_CORR=0 REMOVEDF_INCO=0 DI_CORR=0 DI_INCO=0
for FILE in "$REPO"/build-*/*-passed/*.txt "$REPO"/build/*-failed/*.txt; do
    while IFS='' read -r LINE; do
        RE='\*\*\* Dead Functions: ([0-9]+) correct, ([0-9]+) incorrect'
        if [[ "$LINE" =~ $RE ]]; then
            DF_CORR=$(($DF_CORR + ${BASH_REMATCH[1]}))
            DF_INCO=$(($DF_INCO + ${BASH_REMATCH[2]}))
        fi
        RE='\*\*\* Functions In Modified IL: ([0-9]+) correct, ([0-9]+) incorrect'
        if [[ "$LINE" =~ $RE ]]; then
            REMOVEDF_CORR=$(($REMOVEDF_CORR + ${BASH_REMATCH[1]}))
            REMOVEDF_INCO=$(($REMOVEDF_INCO + ${BASH_REMATCH[2]}))
        fi
        RE='\*\*\* Dead Instructions: ([0-9]+) correct, ([0-9]+) incorrect'
        if [[ "$LINE" =~ $RE ]]; then
            DI_CORR=$(($DI_CORR + ${BASH_REMATCH[1]}))
            DI_INCO=$(($DI_INCO + ${BASH_REMATCH[2]}))
        fi
    done < "$FILE"
done

bc <<< "scale = 4; 
        s1 = 50/4*$DF_CORR - 50/4*$DF_INCO; 
        if (s1 < 0) s1 = 0;
        s2 = 25/7*$REMOVEDF_CORR - 25/7*$REMOVEDF_INCO;
        if (s2 < 0) s2 = 0;
        s3 = 25/3*$DI_CORR - 25/3*$DI_INCO;
        if (s3 < 0) s3 = 0;
        s1 + s2 + s3"
