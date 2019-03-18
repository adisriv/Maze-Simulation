#!/bin/bash
#
# ./mazeTest.sh
#
# usage: ./mazeTest.sh AvatarId, nAvatars, Difficulty, IP address of the server, MazePort, Filename of the log
# (no arguments)
#
# input: AvatarId, nAvatars, Difficulty, IP address of the server, MazePort, Filename of the log
# output: logfile
#
# Group 16, March 2018

echo TESTING MAZE CHALLENGE

echo Edge Cases

echo Usage Error: Missing IP Address
./Avatars 0 2 5 flume Amazing_Group16_2_5.log

echo Usage Error: Missing filename
./Avatars 0 2 5 flume 10829

echo Too Many Avatars
./Avatars 0 100 5 flume 10829 Amazing_Group16_2_5.log

echo Difficulty Too High
./Avatars 0 2 20 flume 10829 Amazing_Group16_2_5.log


echo Solving Maze 0
./Avatars 0 2 0 flume 10829 Amazing_Group16_2_0.log

echo Solving Maze 1
./Avatars 0 3 1 129.170.212.235 17235 Amazing_Group16_3_1.log

echo Solving Maze 2
./Avatars 0 3 2 flume 10829 Amazing_Group16_3_2_1.log

echo Solving Maze 3
./Avatars 0 3 3 flume.cs.dartmouth.edu 17235 Amazing_Group16_3_3_2.log

echo Solving Maze 4
./Avatars 0 2 4 flume 10829 Amazing_Group16_2_4.log

echo Solving Maze 5
./Avatars 0 2 5 flume 10829 Amazing_Group16_2_5.log