#!/bin/bash
mb=$((1024*1024))

rungroup()
{
  local tool=${1}
  local trace=${2}
  mkdir -p trace
  x=${3}
  for i in $(seq 1 24); do
    sz=$(($mb * $x))
    ./${tool} ${trace}/sp ${trace}/vlen $(cat ${trace}/keys) ${sz} > trace/${tool}-${trace}-${x}.txt &
    x=$(($x + ($x / 4)))
  done
  wait
}

rungroup lru etc 20
rungroup lru usr 5
rungroup lru app 10
rungroup lirs etc 20
rungroup lirs usr 5
rungroup lirs app 10
