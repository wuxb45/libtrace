#!/bin/bash
SZ04=4294967296
SZ08=8589934592
SZ12=12884901888
SZ16=17179869184
SZ20=21474836480
SZ24=25769803776
SZ28=30064771072
SZ32=34359738368
SZ36=38654705664
SZ40=42949672960
SZ44=47244640256
SZ48=51539607552
SZ52=55834574848
SZ56=60129542144
SZ60=64424509440
SZ64=68719476736

TRACE=${1}

SP="/mnt/wuxb/${TRACE}.sp"
if [[ ! -f ${SP} ]]; then
  echo "${SP} not found"
  exit 0
fi

VLENY="/mnt/wuxb/${TRACE}_vleny"
if [[ ! -f ${VLENY} ]]; then
  echo "${VLENY} not found"
  exit 0
fi

KEYS="/mnt/wuxb/${TRACE}_keys.txt"
if [[ ! -f ${KEYS} ]]; then
  echo "${KEYS} not found"
  exit 0
fi

mkdir -p lru_${TRACE}
#for sz in $SZ04 $SZ08 $SZ12 $SZ16 $SZ20 $SZ24 $SZ28 $SZ32 $SZ36 $SZ40 $SZ44 $SZ48 $SZ52 $SZ56 $SZ60 $SZ64; do
for sz in $SZ04 $SZ08 $SZ12 $SZ16; do
  echo $sz
  ./lru ${SP} ${VLENY} $(cat ${KEYS}) $sz > lru_${TRACE}/${sz}.txt
done
