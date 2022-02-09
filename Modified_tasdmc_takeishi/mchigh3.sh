#!/bin/bash


if [[ $# -ne 2 ]]
then
    echo 'mchigh.sh [number of threads per node] [memory per thread (GB)]'
    exit
fi
if [[ $1 -lt 2 ]]
then
    echo 'There must be a minimum of 2 GB per thread'
    exit
fi

ulimit -s unlimited

#TEST
export list=$(echo p01 p02 p03 p04 p05 p06)

#EVERYTHING
#export list=$(echo p01 p02 p03 p04 p05 p06 p07 p08 p09 p10 p11 p12 p13 p14 p15 p16 p17 p18 p19 p20 p21 p22 p23 p24 p25 p26 p27 p28 p29 p30 p31 p32 p33 p34 p35 p36 p37 p38 p39 p40 p41 p42 p43 p44 p45 p46 p47 p48 p49 p50 p51 p52 p53 p54 p55 p56 p57 p58 p59 p60 p61)

#SUMMER
#export list=$(echo p01 p02 p03 p04 p05 p12 p13 p14 p15 p16 p17 p24 p25 p26 p27 p28 p29 p36 p37 p38 p39 p40 p41 p48 p49 p50 p51)

#WINTER
#export list=$(echo p06 p07 p08 p09 p10 p11 p18 p19 p20 p21 p22 p23 p30 p31 p32 p33 p34 p35 p42 p43 p44 p45 p46 p47) 

export DATADIR=${TASDMC}/gmd/done/done
export DATA_LOCAL=${LOCAL}/data.${USER}
rm -rf  ${LOCAL}/done.* ${LOCAL}/data.*
rsync -PauL ${TASDMCBIN}/sdmc_spctr_${2}g_noa ${TASDMCDATA}/atmos.bin ${TASDMCBIN}/dstcat $LOCAL
for part in $list
  do
  rsync -Pau ${TASDMCDATA}/sdcalib_`echo $part | sed s/p0// | sed s/p//`.bin $LOCAL
done

mkdir $DATA_LOCAL
mkdir $DATA_LOCAL/pro
ln -s ${LOCAL}/sdmc_spctr_${2}g_noa ${DATA_LOCAL}/.
ln -s ${LOCAL}/atmos.bin ${DATA_LOCAL}/.
ln -s ${LOCAL}/sdcalib_*.bin ${DATA_LOCAL}/.
ln -s ${LOCAL}/dstcat ${DATA_LOCAL}/.
eternity=0
na=0

while [[ $eternity -lt 1 ]]
do
  let doneflag=1
  cd $DATADIR
  for file in `echo DAT??????*.dat 2> /dev/null` 
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
      exit
  fi
  export energy=`echo $file | sed s/_gea.dat// | sed s/DAT....//`
  if [ "$energy" = "30" ]
      then
      export nevt=30773
  elif [ "$energy" = "31" ]
      then
      export nevt=18285
  elif [ "$energy" = "32" ]
      then
      export nevt=10864
  elif [ "$energy" = "33" ]
      then
      export nevt=6455
  elif [ "$energy" = "34" ]
      then
      export nevt=3836
  elif [ "$energy" = "35" ]
      then
      export nevt=2279
  elif [ "$energy" = "36" ]
      then
      export nevt=1354
  elif [ "$energy" = "37" ]
      then
      export nevt=1238
  elif [ "$energy" = "38" ]
      then
      export nevt=1195
  elif [ "$energy" = "39" ]
      then
      export nevt=1136
  elif [ "$energy" = "00" ]
      then
      export nevt=675
  elif [ "$energy" = "01" ]
      then
      export nevt=401
  elif [ "$energy" = "02" ]
      then
      export nevt=238
  elif [ "$energy" = "03" ]
      then
      export nevt=142
  elif [ "$energy" = "04" ]
      then
      export nevt=84
  else
      export nevt=50
  fi
  export tag=$file
  cp pro/${tag} ${DATA_LOCAL}/.
  cd $DATA_LOCAL
  
  for part in $list
    do
    ln -s $tag `echo $tag | sed s/_gea.dat/.${part}/`
  done
  export pre=`echo $tag | sed s/_gea.dat//`
####  
  na=`expr $na + 1`
  echo "na: $na"
#  if test $na -eq 5; then
  if test $na -eq 6; then
####
  let i=0
#  for file in `ls *.p??`
  for file in `ls *_0.p??`
    do
    let j=i+1
    let k=i-${1}+1
#    export part=`echo $file | sed s/${pre}.p//`
#    export pre2=`echo $tag | sed s/_4_gea.dat//`
    export pre2=`echo $tag | sed s/_5_gea.dat//`
    export part=`echo $file | sed s/${pre2}_0.p//`

echo "file: $file"
echo "part: $part"
echo "tag:  $tag"
cp -rp $DATA_LOCAL $TASDMC/temp3/

    until [[ `ls done.* 2> /dev/null | wc -l` -ge $k ]]
      do
      sleep 1
    done
#    (./sdmc_spctr_${2}g_noa $file $nevt `date +%N` $part >& /dev/null ; echo DONE > "done.${i}" ) &
    (./sdmc_spctr_${2}g_noa $file $nevt `date +%N` $part ; echo DONE > "done.${i}" ) &
echo "i: $i"
    let i=i+1
  done
####
  ls -al $DATA_LOCAL
####
  until [[ `ls done.* 2> /dev/null | wc -l` -eq `echo $list | wc -w` ]] 
    do 
    sleep 1
  done
  rm done.*

#   rm *.p?? $tag
#  ./dstcat -o `echo $tag | sed s/.dat/.dst.gz/` *p??.dst.gz >& /dev/null
#  mv `echo $tag | sed s/.dat/.dst.gz/` ${DATADIR}/done/.
#  mv ${DATADIR}/pro/$tag ${DATADIR}/done/.
#  rm `echo ${tag} | sed s/_gea.dat//`*

  rm *.p?? *_gea.dat
  nb=0
  gea="_gea.dst.gz"
  p=".p"
####
  ls -al $DATA_LOCAL
####
  while [ $nb -ne 6 ]
  do
#    ./dstcat -o `echo $tag | sed s/_4_gea.dat/_$nb$gea/` *_$nb$p??.dst.gz >& /dev/null
    ./dstcat -o `echo $tag | sed s/_5_gea.dat/_$nb$gea/` *_$nb$p??.dst.gz >& /dev/null
    nb=`expr $nb + 1`
  done
  rm *.p??.dst.gz
  mv *dst.gz ${DATADIR}/done/.
#  rm `echo ${tag} | sed s/_gea.dat//`*
####
  na=0
  fi
  ls -al $DATA_LOCAL
  mv ${DATADIR}/pro/$tag ${DATADIR}/done/.
####
done
