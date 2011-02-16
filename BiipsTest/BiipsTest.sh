#!/bin/sh

nfailed=0
for var in bench/*.cfg; do
	$1/BiipsTest $var --n-smc=$2
	if [ $? != 0 ]; then
	{
	  failed[nfailed]=$var
	  nfailed=`expr $nfailed + 1`
	}
	fi
done
echo $nfailed "tests failed:"
echo ${failed[@]}
exit 0
