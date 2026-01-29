#!/bin/bash

OD="diff_file"
TG="$1"

mkdir -p "$OD"

./ft_nm "$TG" > "$OD"/"$TG"_ft 2>&1 
LC_ALL=C nm "$TG" > "$OD"/"$TG"_ori 2>&1
