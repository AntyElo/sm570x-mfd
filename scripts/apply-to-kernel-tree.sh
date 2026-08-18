#!/bin/sh
set -eu

if [ $# -ne 1 ]; then
	echo "Usage: $0 /path/to/linux-msm8916" >&2
	exit 2
fi

KDIR="$1"
REPO_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

if [ ! -d "$KDIR/drivers/mfd" ]; then
	echo "Not a Linux kernel tree: $KDIR" >&2
	exit 1
fi

for i in sm570x_fuelgauge.c sm570x_charger.c
do cp "$REPO_DIR/kernel/drivers/mfd/$i" "$KDIR/drivers/mfd/"
done

KCONFIG="$KDIR/drivers/mfd/Kconfig"
MAKEFILE="$KDIR/drivers/mfd/Makefile"

if ! grep -q 'MFD_SM570X' "$KCONFIG"; then
	cat >> "$KCONFIG" < "$REPO_DIR/kernel/drivers/mfd/Kconfig"
fi

if ! grep -q 'sm570x_fuelgauge.o' "$MAKEFILE"; then
	cat >> "$MAKEFILE" < "$REPO_DIR/kernel/drivers/mfd/Makefile"
fi

printf '\nDriver files copied. Now enable these kernel config symbols:\n'
printf '  CONFIG_BATTERY_SM570X_FG=Y\n'
printf '  CONFIG_CHARGER_SM570X=Y\n\n'
printf 'Then merge kernel/dts/sm570x.dtsi into your device DTS.\n'
