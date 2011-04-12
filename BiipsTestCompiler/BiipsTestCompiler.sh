#!/bin/bash

if [[ -s $LD_LIBRARY_PATH ]]
then export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../BiipsCore/$1:../BiipsBase/$1:../BiipsCompiler/$1
else export LD_LIBRARY_PATH=../BiipsCore/$1:../BiipsBase/$1:../BiipsCompiler/$1
fi

ntests=0
nfailed=0
start_time=`date +%s`

for cfg in bench/*.cfg; do
	ntests=`expr $ntests + 1`
	
	$1/BiipsTestCompiler $cfg --n-smc=$2 $3 $4 $5
	
	if [ $? -ne 0 ]; then
	{
	  nfailed=`expr $nfailed + 1`
	  failed[$nfailed]=$cfg
	}
	fi
done

end_time=`date +%s`
elapsed=`expr $end_time - $start_time`

remainder="$(expr $elapsed % 3600)"
hours="$(expr $(expr $elapsed - $remainder) / 3600)"
seconds="$(expr $remainder % 60)"
minutes="$(expr $(expr $remainder - $seconds) / 60)"


echo
echo "**************** REPORT ****************"
echo Finished at `date`
echo Elapsed time: $hours:$minutes:$seconds
echo -n $nfailed/$ntests tests failed
if [ $nfailed -ne 0 ]; then
{
	echo " in files:"
	for cfg in ${failed[*]}; do
		echo -e '\t'$cfg
	done
}
else
{
	echo
}
fi
echo "----------------------------------------"
exit 0
