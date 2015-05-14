#!/bin/sh

PROJECT=$(cd $(dirname $0) && pwd)
export ARTISTIC_STYLE_OPTIONS=${PROJECT}/astylerc
astyle -n --recursive "${PROJECT}/util/*.h" --recursive "${PROJECT}/tests/*.cpp"

