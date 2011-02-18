#!/bin/sh

ntests=0
nfailed=0
for var in bench/*.cfg; do
	ntests=`expr $ntests + 1`
	$1/BiipsTest $var --n-smc=$2
	if [ $? -ne 0 ]; then
	{
	  nfailed=`expr $nfailed + 1`
	}
	fi
done
echo
echo "------------------"
echo $nfailed"/"$ntests "tests failed"
exit 0
