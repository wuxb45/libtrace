#!/bin/bash
mb=$((1024*1024))
bg_run_one()
{
  local tool=${1}
  local trace=${2}
  local nmb=${3}
  local outdir=${4}
  local paramx=${5}

  local sz=$((${mb} * ${nmb}))
  mkdir -p ${outdir}

  ./${tool} ${trace}/sp ${trace}/vlen $(cat ${trace}/keys) ${sz} ${paramx} > ${outdir}/${trace}-${nmb}-${tool}.txt &
}

bg_run_group()
{
  local tool=${1}
  local trace=${2}
  local x=${3}
  local rounds=${4}
  local outdir=${5}
  local paramx=${6}
  
  for i in $(seq 1 ${rounds}); do
    bg_run_one ${tool} ${trace} ${x} ${outdir} ${paramx}
    x=$(($x + ($x / 4)))
  done
}


