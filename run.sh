#!/bin/bash

echo "$1"
for (( s=0; i<($1); s++ )); do
	echo "$s"
	./bin/main -s "$s"
done

