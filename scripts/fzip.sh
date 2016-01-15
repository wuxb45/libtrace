#!/bin/bash
# g{lru,lrux,lirs,arc}
# f{lru,lrux,lirs,arc}
. algocommon.sh

#### 80%
#80% group
glru ()
{
  bg_run_group lru  etc 30 22 group
  bg_run_group lru  usr  8 20 group
  wait
  bg_run_group lru  app 10 24 group
  bg_run_group lru  zip 60 24 group
  wait
}

glrux ()
{
  bg_run_group lrux etc 30 22 group 608372793
  bg_run_group lrux usr  8 20 group 27162703
  wait
  bg_run_group lrux app 10 24 group 504676200
  bg_run_group lrux zip 60 24 group 275014743
  wait
}

glirs ()
{
  bg_run_group lirs etc 30 22 group
  bg_run_group lirs usr  8 20 group
  wait
  bg_run_group lirs app 10 24 group
  bg_run_group lirs zip 60 24 group
  wait
}

garc ()
{
  bg_run_group arc  etc 30 22 group
  bg_run_group arc  usr  8 20 group
  wait
  bg_run_group arc  app 10 24 group
  bg_run_group arc  zip 60 24 group
  wait
}

fzip ()
{
  bg_run_one lru zip 262 points
  bg_run_one lru zip 393 points
  bg_run_one lru zip 525 points
  bg_run_one lru zip 656 points
  bg_run_one lru zip 787 points
  bg_run_one lrux zip 393 points 275014743
  bg_run_one lrux zip 525 points 275014743
  bg_run_one lrux zip 656 points 275014743
  bg_run_one lrux zip 787 points 275014743
  bg_run_one lirs zip 262 points
  bg_run_one lirs zip 393 points
  bg_run_one lirs zip 525 points
  bg_run_one lirs zip 656 points
  bg_run_one lirs zip 787 points
  bg_run_one arc zip 262 points
  bg_run_one arc zip 393 points
  bg_run_one arc zip 525 points
  bg_run_one arc zip 656 points
  bg_run_one arc zip 787 points
  wait
}
${1}
