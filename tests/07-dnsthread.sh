#!/bin/sh

# Goal of this test:
# - ensure that dnsquery can find simple addresses

### Constants
addr_output="${s_basename}-addrs.txt"
addrs="	localhost:80
	1.2.3.4:80
	1:2:3:4:0:a:b:c:80
	1:2:3:4::a:b:c:80
	::1:80
	/tmp/sock"

### Actual command
scenario_cmd() {
	for addr in ${addrs}
	do
		setup_check_variables
		${dnsthread_resolve} ${addr} >> ${addr_output}
		echo $? > ${c_exitfile}
	done

	# Wait for server(s) to quit.
	servers_stop
}
