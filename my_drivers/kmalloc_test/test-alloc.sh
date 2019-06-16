#!/bin/bash

size=$1
pgmajfault=`cat /proc/vmstat | grep pgmajfault | awk '{print $2}'`
compact_success=`cat /proc/vmstat | grep compact_success | awk '{print $2}'`
compact_fail=`cat /proc/vmstat | grep compact_fail | awk '{print $2}'`
free -tm ;
cat /proc/buddyinfo ;
echo "BEFORE - Major Page Fault: "$pgmajfault
echo "BEFORE -     Compact fail: "$compact_fail
echo "BEFORE -  Compact success: "$compact_success
vmstat
echo $size > /dev/pinchar
vmstat
dmesg | tail -n 50
sleep 1
echo "BEFORE - Major Page Fault: "$pgmajfault
echo "BEFORE -     Compact fail: "$compact_fail
echo "BEFORE -  Compact success: "$compact_success
free -tm ;
cat /proc/buddyinfo ;

