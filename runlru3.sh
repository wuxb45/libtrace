#!/bin/bash
. algocommon.sh

###70%
#bg_run_group lru  etc 20 24 dir
#bg_run_group lrux etc 20 24 dir 262171838
#bg_run_group lirs etc 20 24 dir
#
#bg_run_group lru  app 10 24 dir
#bg_run_group lrux app 10 24 dir 258158358
#bg_run_group lirs app 10 24 dir
#
#bg_run_group lru  usr  8 16 dir
#bg_run_group lrux usr  8 16 dir 17199206
#bg_run_group lirs usr  8 16 dir

#### 80%
#80% group
grouplru ()
{
  bg_run_group lru  etc 30 22 noset
  bg_run_group lru  app 10 24 noset
  bg_run_group lru  usr  8 20 noset
  wait
}

grouplrux ()
{
  bg_run_group lrux etc 30 22 noset 608372793
  bg_run_group lrux app 10 24 noset 504676200
  bg_run_group lrux usr  8 20 noset 27162703
  wait
}

grouplirs ()
{
  bg_run_group lirs etc 30 22 noset
  bg_run_group lirs app 10 24 noset
  bg_run_group lirs usr  8 20 noset
  wait
}

grouparc ()
{
  bg_run_group arc  etc 30 22 noset
  bg_run_group arc  app 10 24 noset
  bg_run_group arc  usr  8 20 noset
  wait
}

fixedlru ()
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
  wait
}

fixedlrux ()
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
  wait
}

fixedlirs ()
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
  wait
}

fixedarc ()
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
  wait
}

${1}
