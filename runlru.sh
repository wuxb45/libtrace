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

OP4="/opt/wuxb/${TRACE}.op4"
if [[ ! -f ${OP4} ]]; then
  echo "${OP4} not found"
  exit 0
fi

VLEN1="/opt/wuxb/${TRACE}_vlen1"
if [[ ! -f ${VLEN1} ]]; then
  echo "${VLEN1} not found"
  exit 0
fi

VLEN2="/opt/wuxb/${TRACE}_vlen2"
if [[ ! -f ${VLEN2} ]]; then
  echo "${VLEN2} not found"
  exit 0
fi

VLEN4="/opt/wuxb/${TRACE}_vlen4"
if [[ ! -f ${VLEN4} ]]; then
  echo "${VLEN4} not found"
  exit 0
fi

KEYS="/opt/wuxb/${TRACE}_keys.txt"
if [[ ! -f ${KEYS} ]]; then
  echo "${KEYS} not found"
  exit 0
fi

mkdir -p lru_${TRACE}
#for sz in $SZ04 $SZ08 $SZ12 $SZ16 $SZ20 $SZ24 $SZ28 $SZ32 $SZ36 $SZ40 $SZ44 $SZ48 $SZ52 $SZ56 $SZ60 $SZ64; do
for sz in $SZ32 $SZ40 $SZ48 $SZ56 $SZ64; do
  echo $sz
  ./lru /opt/wuxb/${TRACE}.op4 /opt/wuxb/${TRACE}_vlen1 /opt/wuxb/${TRACE}_vlen2 /opt/wuxb/${TRACE}_vlen4 $(cat ${KEYS}) $sz > lru_${TRACE}/${sz}.txt
done
