#!/bin/bash

echo $1

mmv $1/'*上*' $1/'#1up#2'
mmv $1/'*下*' $1/'#1down#2'
mmv $1/'*右*' $1/'#1right#2'
mmv $1/'*左*' $1/'#1left#2'
mmv $1/'*无*' $1/'#1none#2'
mmv $1/'*正*' $1/'#1face#2'

