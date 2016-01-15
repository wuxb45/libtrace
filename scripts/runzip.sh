#!/bin/bash
. algocommon.sh

ziplru ()
{
  bg_run_group lru  zip 60 24 noset
  wait
}

ziplrux ()
{
  bg_run_group lrux zip 60 24 noset 275014743
  wait
}

ziplirs ()
{
  bg_run_group lirs zip 60 24 noset
  wait
}

ziparc ()
{
  bg_run_group arc  zip 60 24 noset
  wait
}
${1}
