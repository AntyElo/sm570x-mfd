#!/bin/sh
set -eu

CHG=/sys/class/power_supply/sm570x-charger
FG=/sys/class/power_supply/sm570x-fuelgauge

show() {
	echo "=== charger ==="
	for f in online status charge_control_limit charge_control_limit_max charge_type current_now current_max voltage_max; do
		printf '%s=' "$f"
		cat "$CHG/$f" 2>/dev/null || echo missing
	done
	echo "=== fuelgauge ==="
	for f in capacity voltage_now current_now temp present technology status; do
		printf '%s=' "$f"
		cat "$FG/$f" 2>/dev/null || echo missing
	done
}

this() {
	printf '\n$1 charge for 5 seconds...\n'
	echo $2 > "$CHG/charge_control_limit"
	sleep 5
	show
}

show
this Disabling 1
this Enabling  0
