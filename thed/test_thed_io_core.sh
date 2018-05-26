#!/bin/bash

name=$0
thed_bin="./thed"
hex_dump="./thed_test_hex_dump.txt"
hex_back_from="./back_from_hex_test"
csv_dump="./thed_test_csv_dump.txt"
csv_back_from="./back_from_csv_test"
test_f=""

main()
{
	if [ $# -lt 1 ]; then
		echo "Use: $name <test file>"
		exit 1
	fi
	
	if [ ! -e $thed_bin ]; then
		echo "Err: no thed"
		exit 1
	fi
	
	for f in ${@:1}; do
		test_file $f
	done
}

test_file()
{
	test_f=$1
	if [ ! -e $test_f ]; then
		echo "Err: no file $test_f"
		return 1
	fi

	# test hex dump
	$thed_bin $test_f > $hex_dump
	$thed_bin -b $hex_dump $hex_back_from
	diff -s $test_f $hex_back_from
	if [ 0 -eq $? ]; then
		rm $hex_dump
		rm $hex_back_from
	else
		echo "Err: hex test failed for $test_f"
	fi

	# test csv dump
	$thed_bin -c $test_f $csv_dump
	$thed_bin -cb $csv_dump $csv_back_from
	diff -s $test_f $csv_back_from
	if [ 0 -eq $? ]; then
		rm $csv_dump
		rm $csv_back_from
	else
		echo "Err: csv test failed for $test_f"
	fi
}

main $@
