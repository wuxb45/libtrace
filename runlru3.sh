#!/bin/bash
mb=$((1024*1024))

mkdir -p newstat
rungroup()
{
  local tool=${1}
  local trace=${2}
  local x=${3}
  local rounds=${4}
  local paramx=${5}
  
  for i in $(seq 1 ${rounds}); do
    local sz=$(($mb * $x))
    ./${tool} ${trace}/sp ${trace}/vlen $(cat ${trace}/keys) ${sz} ${paramx} > newstat/${trace}-${x}-${tool}.txt &
    x=$(($x + ($x / 4)))
  done
}

rungroup lru etc 20 24
rungroup lrux etc 20 24 262171838
wait
rungroup lirs etc 20 24

rungroup lru app 10 24
wait
rungroup lrux app 10 24 258158358
rungroup lirs app 10 24
wait

rungroup lru usr 8 16
rungroup lrux usr 8 16 17199206
rungroup lirs usr 8 16
wait
