#!/bin/bash

#Bash script for USB to Serial UART adapter

usage() {
	cat <<EOF
	m2k_tty [options]
	Optional arguments:
	-h            		show help message and exit
	-r <value>		set the rx pin, if not set the default value is 7
	-t <value>		set the tx pin, if not set the default value is 15
	-u <value>		set the uri of the m2k, if not set the default value is ip:192.168.2.1
	-i <value>		set the baud_rate/data_bits/parity/stop_bits of the UART communication, 
					if not set the default value is 115200/8/N/1
			uri - describes the context location
			rx - index of any digital pin on m2k (read)
			tx - index of any digital pin on m2k (write)
			baud_rate - int
			parity = {N for none | O for odd | E for even | M for mark | S for space}
			data_bits - {5 | 6 | 7 | 8}
			stop_bits - {1 | 1.5 | 2}
  
EOF
}
terminate_script() {
	kill_pid=$(ps -ef | grep "m2kcli uart-terminal" | grep -v grep | awk '{print $2}')
	if [ -n "${kill_pid}" ]; then
		disown "${kill_pid}"
		kill -9 "${kill_pid}"
		if [ $? -eq 0 ]; then
			echo "-Killed m2kcli uart-terminal"
		else
			echo "Failed to kill: m2kcli uart-terminal "
			exit 1
		fi
	else
		echo "-NO M2KCLI OPEN"
	fi

	kill_pid=$(ps -ef | grep "socat -d -d pty,raw,echo=0 pty,raw,echo=0" | grep -v grep | awk '{print $2}')
	if [ -n "${kill_pid}" ]; then

		disown "${kill_pid}"
		kill -9 "${kill_pid}"
		if [ $? -eq 0 ]; then
			echo "-Killed socat"
		else
			echo "Failed to kill: socat "
			exit 1
		fi
	else
		echo "-NO SOCAT OPEN"
	fi

	echo "--Connection closed"
}

if [ -f "${temp_file}" ]; then
	rm "${temp_file}"
fi
if [ -f "${log_file}" ]; then
	rm "${log_file}"
fi

while getopts u:i:r:t:h option; do
	case "${option}" in
	u)
		uri=${OPTARG}
		;;
	i)
		settings=${OPTARG}
		;;
	r)
		rx=${OPTARG}
		;;
	t)
		tx=${OPTARG}
		;;
	h)
		usage
		exit 1
		;;
	\?)
		echo "Options problem"
		exit 1
		;;
	esac
done
shift "$((OPTIND - 1))"

uri=${uri:-ip:192.168.2.1}
settings=${settings:-115200/8/N/1}
rx=${rx:-7}
tx=${tx:-15}

log_file=/tmp/m2k_tty-log.txt
temp_file=/tmp/m2k_tty-temp.txt

mapfile -t settings < <(grep -Eo '[0-9]+|[A-Z]|[a-z]' <<<"${settings}")
baud_rate=${settings[0]}
bits_number=${settings[1]}
case ${settings[2]} in
N)
	parity=none
	;;
O)
	parity=odd
	;;
E)
	parity=even
	;;
M)
	parity=mark
	;;
S)
	parity=space
	;;
*)
	parity=none
	;;
esac
stop_bits=${settings[3]}

echo "Creating the PTYs..."
socat -d -d pty,raw,echo=0 pty,raw,echo=0 2>${temp_file} &
PID=$!
while ! [ -f ${temp_file} ]; do sleep 1; done
sleep 2

if ps -p $PID > /dev/null
then
	echo "PTYs created successfully"
else
	echo
	echo "Problem at creating PTYs"
	cp ${temp_file} ${log_file}
	echo
	echo "For details check logs at: ${log_file}"
	echo
	rm "${temp_file}"
	terminate_script
	exit 1
fi

mapfile -t path < <(grep -o '/dev/pts/.*' "${temp_file}")

m2kcli uart-terminal "${uri}" -i baud_rate="${baud_rate}" rx="${rx}" tx="${tx}" stop_bits="${stop_bits}" parity="${parity}" bits_number="${bits_number}" <"${path[0]}" | tee -a "${temp_file}" >"${path[0]}" &
PID=$!

echo "Checking uart-terminal..."
sleep 5

if ps -p $PID > /dev/null
then
	echo "Uart-terminal created successfully"
else
	echo
	echo "Problem at creating uart-terminal"
	cp ${temp_file} ${log_file}
	echo
	echo "For details check logs at: ${log_file}"
	echo
	rm "${temp_file}"
	terminate_script
	exit 1
fi

cat <<EOF
M2K configurations:
	URI: ${uri}
	RX pin: ${rx}
	TX pin: ${tx}
Sample details for UART:
	Baud rate: ${baud_rate}
	Number of data bits: ${bits_number}
	Parity: ${parity}
	Stop bits: ${stop_bits}

Done, connect to ${path[1]} to access the uart terminal

EOF

read -p "Press any key to close the connection"
terminate_script
rm "${temp_file}"
exit 0
