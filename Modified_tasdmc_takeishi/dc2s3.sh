#!/bin/bash

if [[ $# -ne 2 ]]
then
    echo 'dc2s.sh [number of threads per node] [memory per node (GB)]'
    exit
fi

ulimit -s unlimited

export DATADIR=${TASDMC}/gmd/done
rm -rf ${LOCAL}/data.*
export DATA_LOCAL=${LOCAL}/data.${USER}
mkdir $DATA_LOCAL
cd $DATA_LOCAL
rsync -Pau ${TASDMCBIN}/corsika2geant_${2}g ${TASDMCDATA}/sdgeant.dst ${TASDMCBIN}/deweight  ${TASDMCBIN}/corsika_split-th $LOCAL >& /dev/null
ln -s ${LOCAL}/corsika2geant_${2}g .
ln -s ${LOCAL}/sdgeant.dst .
ln -s ${LOCAL}/deweight .
ln -s ${LOCAL}/corsika_split-th .
eternity=0

while [[ $eternity -lt 1 ]]
do
    let doneflag=1
    cd $DATADIR
    
    for file in `echo DAT??????_dc06 2> /dev/null` 
    do
	if mv $file pro 
	then 
	    touch pro/${file}
	    let doneflag=0
	    break 
	fi 
    done
    if [[ $doneflag -eq 1 ]]
    then
	echo DONE > ${LOCAL}/done.`date +%N`
	exit
    fi
    export tag=`echo $file | sed s/_dc06//`
    cp ${DATADIR}/pro/$file ${DATA_LOCAL}/.
    cd $DATA_LOCAL
####
ls -al $DATA_LOCAL
####
    ./corsika_split-th ${file} $1 >& /dev/null
####
ls -al $DATA_LOCAL
####
    let i=0
    for file in `ls *.p??`
      do
      (./deweight $file >& /dev/null ; echo DONE > "done.${i}" ) &
      let i=i+1
    done
####
ls -al $DATA_LOCAL
####
    until [ `ls done.* 2> /dev/null | wc -l` = "${1}" ] 
      do 
      sleep 10 
    done
    ls ${tag}*dwt* > ${tag}
####
ls -al $DATA_LOCAL
#    cp ${tag} $DATADIR/done/.
cp -rp $DATA_LOCAL $TASDMC/temp2/
#####
     ./corsika2geant_${2}g ${tag} sdgeant.dst >& /dev/null
####
ls -al $DATA_LOCAL
####
     cp ${tag}_*gea.dat $DATADIR/done/.
    rm -rf ${tag}* done.* tmp???
    mv ${DATADIR}/pro/${tag}_dc06 ${DATADIR}/done/.
done
