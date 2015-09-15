#!/bin/bash
# g_{lru,lrux,lirs,arc}
# f_{lru,lrux,lirs,arc}_{etc,usr,app,zip}
. algocommon.sh

# group by algo
g_lru ()
{
  bg_run_group lru  etc 30 22 group
  bg_run_group lru  usr  8 16 group
  wait
  bg_run_group lru  app 10 24 group
  bg_run_group lru  zip 60 24 group
  wait
}

g_lrux ()
{
  bg_run_group lrux etc 30 22 group 608372793
  bg_run_group lrux usr  8 16 group 27162703
  wait
  bg_run_group lrux app 10 24 group 504676200
  bg_run_group lrux zip 60 24 group 127012516
  wait
}

g_lirs ()
{
  bg_run_group lirs etc 30 22 group
  bg_run_group lirs usr  8 16 group
  wait
  bg_run_group lirs app 10 24 group
  bg_run_group lirs zip 60 24 group
  wait
}

g_arc ()
{
  bg_run_group arc  etc 30 22 group
  bg_run_group arc  usr  8 16 group
  wait
  bg_run_group arc  app 10 24 group
  bg_run_group arc  zip 60 24 group
}

# group by trace
g_etc ()
{
  bg_run_group lru  etc 30 22 group
  bg_run_group arc  etc 30 22 group
  wait
  bg_run_group lrux etc 30 22 group 608372793
  bg_run_group lirs etc 30 22 group
  wait
}

g_usr ()
{
  bg_run_group lru  usr  8 16 group
  bg_run_group arc  usr  8 16 group
  wait
  bg_run_group lrux usr  8 16 group 27162703
  bg_run_group lirs usr  8 16 group
  wait
}

g_app ()
{
  bg_run_group lru  app 10 24 group
  bg_run_group arc  app 10 24 group
  wait
  bg_run_group lrux  app 10 24 group
  bg_run_group lirs  app 10 24 group
  wait
}

g_zip ()
{
  bg_run_group lru  zip 10 24 group
  bg_run_group arc  zip 10 24 group
  wait
  bg_run_group lrux  zip 10 24 group
  bg_run_group lirs  zip 10 24 group
  wait
}

#80% fixed points x1 x1.5 x2 x2.5 x3

f_lru_etc ()
{
  #etc
  bg_run_one lru etc 580 points
  bg_run_one lru etc 870 points
  bg_run_one lru etc 1160 points
  bg_run_one lru etc 1450 points
  bg_run_one lru etc 1740 points
}

f_lru_app ()
{
  #app
  bg_run_one lru app 481 points
  bg_run_one lru app 722 points
  bg_run_one lru app 963 points
  bg_run_one lru app 1203 points
  bg_run_one lru app 1444 points
}

f_lru_usr ()
{
  #usr
  bg_run_one lru usr 26 points
  bg_run_one lru usr 39 points
  bg_run_one lru usr 52 points
  bg_run_one lru usr 65 points
  bg_run_one lru usr 78 points
}

f_lru_zip ()
{
  #zip
  bg_run_one arc zip 121 points
  bg_run_one arc zip 182 points
  bg_run_one arc zip 242 points
  bg_run_one arc zip 303 points
  bg_run_one arc zip 363 points
}

f_lrux_etc ()
{
  bg_run_one lrux etc 870 points 608372793
  bg_run_one lrux etc 1160 points 608372793
  bg_run_one lrux etc 1450 points 608372793
  bg_run_one lrux etc 1740 points 608372793
}

f_lrux_app ()
{
  bg_run_one lrux app 722 points 504676200
  bg_run_one lrux app 963 points 504676200
  bg_run_one lrux app 1203 points 504676200
  bg_run_one lrux app 1444 points 504676200
}

f_lrux_usr ()
{
  bg_run_one lrux usr 39 points 27162703
  bg_run_one lrux usr 52 points 27162703
  bg_run_one lrux usr 65 points 27162703
  bg_run_one lrux usr 78 points 27162703
}

f_lrux_zip ()
{
  bg_run_one lrux zip 182 points 127012516
  bg_run_one lrux zip 242 points 127012516
  bg_run_one lrux zip 303 points 127012516
  bg_run_one lrux zip 363 points 127012516
}

f_lirs_etc ()
{
  bg_run_one lirs etc 580 points
  bg_run_one lirs etc 870 points
  bg_run_one lirs etc 1160 points
  bg_run_one lirs etc 1450 points
  bg_run_one lirs etc 1740 points
}

f_lirs_app ()
{
  bg_run_one lirs app 481 points
  bg_run_one lirs app 722 points
  bg_run_one lirs app 963 points
  bg_run_one lirs app 1203 points
  bg_run_one lirs app 1444 points
}

f_lirs_usr ()
{
  bg_run_one lirs usr 26 points
  bg_run_one lirs usr 39 points
  bg_run_one lirs usr 52 points
  bg_run_one lirs usr 65 points
  bg_run_one lirs usr 78 points
}

f_lirs_zip ()
{
  bg_run_one lirs zip 121 points
  bg_run_one lirs zip 182 points
  bg_run_one lirs zip 242 points
  bg_run_one lirs zip 303 points
  bg_run_one lirs zip 363 points
}

f_arc_etc ()
{
  bg_run_one arc etc 580 points
  bg_run_one arc etc 870 points
  bg_run_one arc etc 1160 points
  bg_run_one arc etc 1450 points
  bg_run_one arc etc 1740 points
}

f_arc_app ()
{
  bg_run_one arc app 481 points
  bg_run_one arc app 722 points
  bg_run_one arc app 963 points
  bg_run_one arc app 1203 points
  bg_run_one arc app 1444 points
}

f_arc_usr ()
{
  bg_run_one arc usr 26 points
  bg_run_one arc usr 39 points
  bg_run_one arc usr 52 points
  bg_run_one arc usr 65 points
  bg_run_one arc usr 78 points
}

f_arc_zip ()
{
  bg_run_one arc zip 121 points
  bg_run_one arc zip 182 points
  bg_run_one arc zip 242 points
  bg_run_one arc zip 303 points
  bg_run_one arc zip 363 points
}

${1}
