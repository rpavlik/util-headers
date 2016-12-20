#!/bin/sh
# Run with --update to parse headers and generate the guid.list.txt and guid.transforms.txt data files.
# Running with any other arguments will lead to those arguments being treated as input files passed to
# a pipeline starting with grep, that will output the name of any util-headers file found (and its original
# name in this repo)

set -e
DATADIR=$(cd $(dirname $0) && pwd)
HEADERDIR=$(cd $(dirname $0) && cd util && pwd)

defineLineToGUID() {
	sed -n 's/[#]ifndef INCLUDED_.*_GUID_\(.*\)$/\1/p' $@
}

if [ "x$1" = "x--update" ]; then
	echo "Updating GUID databases."
	(
		cd ${HEADERDIR}
		# Generate file to use with sed
		(find * -name "*.h" | xargs grep "#ifndef INCLUDED_" )| defineLineToGUID | sed 's/\([^:]*\):\(.*\)/s:\2:\1:/' > ${DATADIR}/guid.transforms.txt
		# Generate input for grep
		cut --delimiter=: --fields=2 ${DATADIR}/guid.transforms.txt > ${DATADIR}/guid.list.txt
	)

	exit 0
fi

grep -F -f ${DATADIR}/guid.list.txt $@ | defineLineToGUID | sed -f ${DATADIR}/guid.transforms.txt | sed 's/:/\t/'
