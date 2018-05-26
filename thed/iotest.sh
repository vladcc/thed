#!/bin/bash
files=$(find ./test_files/ -type f)
files="${files} ./thed"

./test_thed_io_core.sh $files 2>/dev/null | grep Err

if [ $? -eq 1 ]; then
	# grep didn't find Err
	echo "Success"
fi
