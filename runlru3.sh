#!/bin/bash
mb=$((1024*1024))

rungroup()
{
  trace=${1}
  mkdir -p trace
    x=${2}
  for i in $(seq 1 24); do
    sz=$(($mb * $x))
    ./lru ${trace}/sp ${trace}/vlen $(cat ${trace}/keys) ${sz} > trace/${trace}-${x}.txt &
    x=$(($x + ($x / 4)))
  done
  wait
}

rungroup etc 20
rungroup usr 5
rungroup app 10
