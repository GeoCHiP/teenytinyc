#!/usr/bin/sh

for i in $(ls examples/*); do
	base_name="$(basename -s .teeny $i)"
	rm -rf "$base_name"
	rm -rf "${base_name}.c"
done
