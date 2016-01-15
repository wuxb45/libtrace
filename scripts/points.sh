#!/bin/bash
. algocommon.sh

#80% fixed points x1 x1.5 x2 x2.5 x3
#etc
bg_run_one lru etc 580 points
bg_run_one lru etc 870 points
bg_run_one lru etc 1160 points
bg_run_one lru etc 1450 points
bg_run_one lru etc 1740 points

bg_run_one lirs etc 580 points
bg_run_one lirs etc 870 points
bg_run_one lirs etc 1160 points
bg_run_one lirs etc 1450 points
bg_run_one lirs etc 1740 points

bg_run_one lrux etc 870 points 608372793
bg_run_one lrux etc 1160 points 608372793
bg_run_one lrux etc 1450 points 608372793
bg_run_one lrux etc 1740 points 608372793

#app
bg_run_one lru app 481 points
bg_run_one lru app 722 points
bg_run_one lru app 963 points
bg_run_one lru app 1203 points
bg_run_one lru app 1444 points

bg_run_one lirs app 481 points
bg_run_one lirs app 722 points
bg_run_one lirs app 963 points
bg_run_one lirs app 1203 points
bg_run_one lirs app 1444 points

bg_run_one lrux app 722 points 504676200
bg_run_one lrux app 963 points 504676200
bg_run_one lrux app 1203 points 504676200
bg_run_one lrux app 1444 points 504676200

#usr
bg_run_one lru usr 26 points
bg_run_one lru usr 39 points
bg_run_one lru usr 52 points
bg_run_one lru usr 65 points
bg_run_one lru usr 78 points

bg_run_one lirs usr 26 points
bg_run_one lirs usr 39 points
bg_run_one lirs usr 52 points
bg_run_one lirs usr 65 points
bg_run_one lirs usr 78 points

bg_run_one lrux usr 39 points 27162703
bg_run_one lrux usr 52 points 27162703
bg_run_one lrux usr 65 points 27162703
bg_run_one lrux usr 78 points 27162703
wait
