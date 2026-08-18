# pmaports integration notes

In the tested setup, the kernel package directory was:

```text
~/.local/var/pmbootstrap/cache_git/pmaports/device/testing/linux-postmarketos-qcom-msm8916/
```

The kernel source checkout was:

```text
~/j5x-power-port/linux-msm8916
```

Recommended flow:

1. Copy driver sources into the kernel tree with `scripts/apply-to-kernel-tree.sh`.
2. Enable:

```text
CONFIG_BATTERY_SM570X_FG=Y
CONFIG_CHARGER_SM570X=Y
```

3. Merge `kernel/dts/sm570x.dtsi` into the device DTS.
4. Generate pmaports patch files from the kernel tree:

```sh
cd ~/j5x-power-port/linux-msm8916
PKGDIR=~/.local/var/pmbootstrap/cache_git/pmaports/device/testing/linux-postmarketos-qcom-msm8916

P=drivers/mfd
git add -N $P/sm570x_fuelgauge.c $P/sm570x_charger.c $P/sm570x_irq.c $P/sm570x_core.c $P/sm570x_core.h

## git diff -- arch/arm64/boot/dts/qcom/msm8916-samsung-j5x.dts > "$PKGDIR/0001-arm64-dts-qcom-msm8916-samsung-j5x-add-sm570x-nodes.patch" # -- ???
git diff -- drivers/mfd/Kconfig drivers/mfd/Makefile drivers/power/supply/sm570x_fuelgauge.c > "$PKGDIR/0002-power-supply-add-sm570x-fuelgauge-readonly.patch"
git diff -- drivers/mfd/Kconfig drivers/mfd/Makefile drivers/mfd/sm570x_charger.c > "$PKGDIR/0003-power-supply-add-sm570x-charger-charge-control.patch"
```

5. Add patch filenames to `APKBUILD` if not already listed, then run:

```sh
pmbootstrap checksum linux-postmarketos-qcom-msm8916
pmbootstrap build --force linux-postmarketos-qcom-msm8916
```
