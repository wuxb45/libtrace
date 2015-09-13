#!/bin/bash
# g{lru,lrux,lirs,arc}
# f{lru,lrux,lirs,arc}
. algocommon.sh

#### 80%
#80% group
glru ()
{
  bg_run_group lru  etc 30 22 noset
  bg_run_group lru  usr  8 20 noset
  wait
  bg_run_group lru  app 10 24 noset
  bg_run_group lru  zip 60 24 noset
  wait
}

glrux ()
{
  bg_run_group lrux etc 30 22 noset 608372793
  bg_run_group lrux usr  8 20 noset 27162703
  wait
  bg_run_group lrux app 10 24 noset 504676200
  bg_run_group lrux zip 60 24 noset 275014743
  wait
}

glirs ()
{
  bg_run_group lirs etc 30 22 noset
  bg_run_group lirs usr  8 20 noset
  wait
  bg_run_group lirs app 10 24 noset
  bg_run_group lirs zip 60 24 noset
  wait
}

garc ()
{
  bg_run_group arc  etc 30 22 noset
  bg_run_group arc  usr  8 20 noset
  wait
  bg_run_group arc  app 10 24 noset
  bg_run_group arc  zip 60 24 noset
  wait
}

flru ()
{
  #80% fixed points x1 x1.5 x2 x2.5 x3
  #etc
  bg_run_one lru etc 580 points
  bg_run_one lru etc 870 points
  bg_run_one lru etc 1160 points
  bg_run_one lru etc 1450 points
  bg_run_one lru etc 1740 points
  #app
  bg_run_one lru app 481 points
  bg_run_one lru app 722 points
  bg_run_one lru app 963 points
  bg_run_one lru app 1203 points
  bg_run_one lru app 1444 points
  #usr
  bg_run_one lru usr 26 points
  bg_run_one lru usr 39 points
  bg_run_one lru usr 52 points
  bg_run_one lru usr 65 points
  bg_run_one lru usr 78 points
  #zip
  bg_run_one lru zip 262 points
  bg_run_one lru zip 393 points
  bg_run_one lru zip 525 points
  bg_run_one lru zip 656 points
  bg_run_one lru zip 787 points
  wait
}

flrux ()
{
  bg_run_one lrux etc 870 points 608372793
  bg_run_one lrux etc 1160 points 608372793
  bg_run_one lrux etc 1450 points 608372793
  bg_run_one lrux etc 1740 points 608372793

  bg_run_one lrux app 722 points 504676200
  bg_run_one lrux app 963 points 504676200
  bg_run_one lrux app 1203 points 504676200
  bg_run_one lrux app 1444 points 504676200

  bg_run_one lrux usr 39 points 27162703
  bg_run_one lrux usr 52 points 27162703
  bg_run_one lrux usr 65 points 27162703
  bg_run_one lrux usr 78 points 27162703

  bg_run_one lrux zip 393 points 275014743
  bg_run_one lrux zip 525 points 275014743
  bg_run_one lrux zip 656 points 275014743
  bg_run_one lrux zip 787 points 275014743
  wait
}

flirs ()
{
  bg_run_one lirs etc 580 points
  bg_run_one lirs etc 870 points
  bg_run_one lirs etc 1160 points
  bg_run_one lirs etc 1450 points
  bg_run_one lirs etc 1740 points

  bg_run_one lirs app 481 points
  bg_run_one lirs app 722 points
  bg_run_one lirs app 963 points
  bg_run_one lirs app 1203 points
  bg_run_one lirs app 1444 points

  bg_run_one lirs usr 26 points
  bg_run_one lirs usr 39 points
  bg_run_one lirs usr 52 points
  bg_run_one lirs usr 65 points
  bg_run_one lirs usr 78 points

  bg_run_one lirs zip 262 points
  bg_run_one lirs zip 393 points
  bg_run_one lirs zip 525 points
  bg_run_one lirs zip 656 points
  bg_run_one lirs zip 787 points
  wait
}

farc ()
{
  bg_run_one arc etc 580 points
  bg_run_one arc etc 870 points
  bg_run_one arc etc 1160 points
  bg_run_one arc etc 1450 points
  bg_run_one arc etc 1740 points

  bg_run_one arc app 481 points
  bg_run_one arc app 722 points
  bg_run_one arc app 963 points
  bg_run_one arc app 1203 points
  bg_run_one arc app 1444 points

  bg_run_one arc usr 26 points
  bg_run_one arc usr 39 points
  bg_run_one arc usr 52 points
  bg_run_one arc usr 65 points
  bg_run_one arc usr 78 points

  bg_run_one arc zip 262 points
  bg_run_one arc zip 393 points
  bg_run_one arc zip 525 points
  bg_run_one arc zip 656 points
  bg_run_one arc zip 787 points
  wait
}
${1}
