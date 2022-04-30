#!/usr/bin/env bash

OBJSIZE=1048576
NUMOBJS=500
WORKDIR=`pwd`
APP="build/app/bench/bench_sequential_swap"

RATIO=(0.2 0.4 0.6 0.8 2.0)

function set_cgroup {
	echo $1 | sudo tee /sys/fs/cgroup/memory/dialga/memory.limit_in_bytes
}

function trial {
	ratio=$1
	memlimit=`python3 -c "print(int($ratio * $OBJSIZE * $NUMOBJS))"`
	echo "memlimit: " $memlimit

	set_cgroup $memlimit

	sudo cgexec -g memory:dialga ${APP} --num_objects $NUMOBJS

	sleep 1
}

function main {
	for ratio in ${RATIO[@]}; do
		echo "ratio:" $ratio
		trial $ratio
	done
}

main
