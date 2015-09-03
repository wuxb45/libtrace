#!/bin/bash

mkdir -p output
for sz in 2 4 6 8 12 16 20 24; do
  echo "==${sz}=="
  ./lru trace/sp trace/vlen $(cat trace/keys) ${sz}00000000 > output/${sz}.txt &
done
wait
