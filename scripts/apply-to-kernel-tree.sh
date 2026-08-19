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

for i in "$REPO_DIR/kernel/drivers/mfd/sm570x*"
do cp "$i" "$KDIR/drivers/mfd/"
done

KCONFIG="$KDIR/drivers/mfd/Kconfig"
if ! grep -q 'MFD_SM570X' "$KCONFIG"; then
	cat >> "$KCONFIG" < "$REPO_DIR/kernel/drivers/mfd/Kconfig"
fi

MAKEFILE="$KDIR/drivers/mfd/Makefile"
if ! grep -q 'sm570x_fuelgauge.o' "$MAKEFILE"; then
	cat >> "$MAKEFILE" < "$REPO_DIR/kernel/drivers/mfd/Makefile"
fi

cat <<THATSINFO

Driver files copied. Now enable these kernel config symbols:
  CONFIG_BATTERY_SM570X_FG=Y
  CONFIG_CHARGER_SM570X=Y

Then merge kernel/dts/sm570x.dtsi into your device DTS.
THATSINFO