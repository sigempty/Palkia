#!/usr/bin/env bash

OBJSIZE=1048576
NUMOBJS=500
COMM="rdma"
WORKDIR=`pwd`
APP="build/app/bench/bench_sequential_remote"
SERVER="rdma0.danyang-05:12000"

RATIO=(0.2 0.4 0.6 0.8 2.0)

function start_server {
	ssh danyang-05 "${WORKDIR}/build/src/palkia_service/palkia_service -logtostderr 0 --comm ${COMM} --buf_num 1000 --buf_size 1148576 --send_wq_depth 128 --recv_wq_depth 128 &" &
}

function kill_server {
	ssh danyang-05 "pkill palkia_service"
}

function set_cgroup {
	echo $1 | sudo tee /sys/fs/cgroup/memory/dialga/memory.limit_in_bytes
}

function trial {
	ratio=$1
	memlimit=`python3 -c "print(int($ratio * $OBJSIZE * $NUMOBJS))"`
	echo "memlimit: " $memlimit

	set_cgroup $memlimit

	start_server
	sleep 2

	export PALKIA_MEMORY_WATERMARK=$memlimit
	${APP} --connect ${SERVER} --comm $COMM --buf_num 1000 --buf_size 1148576 --num_objects $NUMOBJS --send_wq_depth 128 --recv_wq_depth 128

	kill_server
	sleep 2
}

function main {
	for ratio in ${RATIO[@]}; do
		echo "ratio:" $ratio
		trial $ratio
	done
}

main
