#!/bin/bash

echo "Clean up....."

# cleanup first
sudo rmmod psi_kernel_test
sudo killall -2 trace-cmd
killall -9 psi_user_test
killall -9 test_video.sh
killall -9 ffmpeg
sudo rm -rf trace*

echo "Start...."

sudo trace-cmd record -e psi_monitor:psi_monitor_top_task &
./mklatencyplot.bash
#sudo trace-cmd record \
#-e psi_monitor:psi_monitor_top_task \
#-e sched:sched_switch \
#-e sched:sched_wakeup \
#-e workqueue:workqueue_execute_start \
#-e workqueue:workqueue_execute_end &

#sudo stress-ng -a 2 --cpu-load 80 --timeout 2m &
stress-ng --cpu 5 --vm 2 --io 2 --cpu-load 60 --timeout 1m &
#sleep 5s
#./test_video.sh &
sleep 5s
./mklatencyplot.bash
./psi_user_test &

sleep 5s
./mklatencyplot.bash
sudo insmod ./driver_test/psi_kernel_test.ko
sleep 5s
./mklatencyplot.bash
sleep 40s
./mklatencyplot.bash
echo "All test done..."

sudo killall -2 trace-cmd
sudo trace-cmd report > trace_report.txt
./mklatencyplot.bash
sudo rmmod psi_kernel_test

killall -9 psi_user_test
killall -9 test_video.sh
killall -9 ffmpeg

echo "Finished..."
./mklatencyplot.bash
