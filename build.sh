#!/usr/bin/sh

set -euo pipefail

COMPILER="$(realpath ./teenytinyc)"
CC="gcc"

function compile {
	local source_file="$1"

	base_name="$(basename -s .teeny "$source_file")"

	"$COMPILER" "$source_file"

	mv out.c "$base_name.c"

	"${CC}" -o "${base_name}" "${base_name}.c"
}

if [ "$#" -eq 0 ]; then
	for i in $(ls examples/*.teeny); do
		compile "$i"
	done
else
	compile "$1"
fi
