#!/bin/sh

# Goal of this test:
# - create a pair of spiped servers (encryption, decryption)
# - establish a connection to the encryption spiped server
# - open one connection, send lorem-send.txt, close the connection
# - the received file should match lorem-send.txt

### Constants
total=$(echo "2^36"|bc)
s=/tmp/spiped-socket

run() {
	bs=$1

	check_leftover_servers
	setup_check_variables

	count=$((${total}/${bs}))
	
	#valgrind --tool=callgrind --callgrind-out-file=recv.callgrind \
	${recv_zeros} ${s} ${bs} &
	nc_pid=${!}

	sleep 1

	#valgrind --tool=callgrind --callgrind-out-file=send.callgrind \
	${send_zeros} ${s} ${bs} ${count}
	echo $? > ${c_exitfile}

	# Wait for server(s) to quit.
	servers_stop
}


### Actual command
scenario_cmd() {
	printf "\n"
	printf "Sending $((${total}/1000000)) Mb via send-zeros -> recv-zeros\n"
	printf "block\tcount\ttime (s)\tspeed (Mb/s)\n"

	#for bs in 8192
	for bs in 512 1024 2048 4096 8192
	do
		run ${bs}
	done
}
