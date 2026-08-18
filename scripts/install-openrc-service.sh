#!/bin/sh
set -eu

mkdir -p /usr/local/sbin /etc/conf.d /etc/init.d

cat > /etc/conf.d/sm570x-store-mode <<'CONF_EOF'
LOW=40
HIGH=60
INTERVAL=60

CAP_PATH=/sys/class/power_supply/sm570x-fuelgauge/capacity
LIMIT_PATH=/sys/class/power_supply/sm570x-charger/charge_control_limit
CURRENT_PATH=/sys/class/power_supply/sm570x-fuelgauge/current_now
TEMP_PATH=/sys/class/power_supply/sm570x-fuelgauge/temp

ENABLE_CHARGE_ON_STOP=yes
CONF_EOF

cat > /usr/local/sbin/sm570x-store-mode <<'DAEMON_EOF'
#!/bin/sh
set -eu

. /etc/conf.d/sm570x-store-mode

log(){ logger -t sm570x-store-mode "$*" 2>/dev/null || true; echo "sm570x-store-mode: $*"; }

while [ ! -e "$CAP_PATH" ] || [ ! -e "$LIMIT_PATH" ]; do log "waiting for power_supply sysfs"; sleep 5; done
log "started: low=$LOW high=$HIGH interval=${INTERVAL}s"

while true; do
	cap="$(cat "$CAP_PATH" 2>/dev/null || echo "")"
	lim="$(cat "$LIMIT_PATH" 2>/dev/null || echo "")"
	cur="$(cat "$CURRENT_PATH" 2>/dev/null || echo unknown)"
	temp="$(cat "$TEMP_PATH" 2>/dev/null || echo unknown)"
	case "$cap:$lim" in *[!0-9:]*|:*|*:) log "invalid reading: cap=$cap limit=$lim"; sleep "$INTERVAL"; continue;; esac
	if [ "$cap" -ge "$HIGH" ] && [ "$lim" -eq 0 ]; then echo 1 > "$LIMIT_PATH"; log "charge disabled: cap=$cap current=$cur temp=$temp"; elif [ "$cap" -le "$LOW" ] && [ "$lim" -eq 1 ]; then echo 0 > "$LIMIT_PATH"; log "charge enabled: cap=$cap current=$cur temp=$temp"; fi
	sleep "$INTERVAL"
done
DAEMON_EOF
chmod +x /usr/local/sbin/sm570x-store-mode

cat > /etc/init.d/sm570x-store-mode <<'INIT_EOF'
#!/sbin/openrc-run

description="SM5703/SM5705 battery store mode 40-60%"
command="/usr/local/sbin/sm570x-store-mode"
command_background="yes"
pidfile="/run/sm5703-store-mode.pid"

depend(){ need localmount; after modules; }

stop_post(){ . /etc/conf.d/sm570x-store-mode; if [ "${ENABLE_CHARGE_ON_STOP:-yes}" = "yes" ] && [ -e "$LIMIT_PATH" ]; then echo 0 > "$LIMIT_PATH"; ewarn "Charging re-enabled on service stop"; fi; }
INIT_EOF
chmod +x /etc/init.d/sm570x-store-mode

rc-update add sm570x-store-mode default 2>/dev/null || true
/etc/init.d/sm570x-store-mode restart
/etc/init.d/sm570x-store-mode status

echo "cap=$(cat /sys/class/power_supply/sm570x-fuelgauge/capacity) limit=$(cat /sys/class/power_supply/sm570x-charger/charge_control_limit) current=$(cat /sys/class/power_supply/sm570x-fuelgauge/current_now) temp=$(cat /sys/class/power_supply/sm570x-fuelgauge/temp)"
