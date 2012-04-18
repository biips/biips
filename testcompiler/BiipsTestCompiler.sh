#!/bin/bash
#                                               -*- bash -*-
#
#  BiiPS software is a set of C++ libraries for
#  Bayesian inference with interacting Particle Systems.
#  Copyright (C) Inria, 2012
#  Contributors: Adrien Todeschini, Francois Caron
#  
#  BiiPS is derived software based on:
#  JAGS, Copyright (C) Martyn Plummer, 2002-2010
#  SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
#
#  This file is part of BiiPS.
#
#  BiiPS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
#  \file     BiipsTestCompiler.sh
#  \brief    bash script for launching BiipsTestCompiler
#            with multiple config files
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#

ntests=0
nfailed=0
start_time=`date +%s`

for cfg in bench/*.cfg; do
	ntests=`expr $ntests + 1`
	
	bin/BiipsTestCompiler $cfg $1 $2 $3 $4 $5
	
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
