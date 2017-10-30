#!/bin/sh

# Goal of this test:
# - ensure that dnsquery can find simple addresses

### Constants
addr_output="${s_basename}-addrs.txt"


### Actual command
scenario_cmd() {
	# Check address
	setup_check_variables
	(
		${dnsthread_resolve} localhost:80 > ${addr_output}
		echo $? > ${c_exitfile}
	)

	# Wait for server(s) to quit.
	servers_stop
}
