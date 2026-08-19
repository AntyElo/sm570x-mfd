#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include "sm570x-core.h"

struct sm570x_fuelgauge {
    struct device *dev;
    struct regmap *regmap;
    struct power_supply *psy;
};

static int sm570x_fuelgauge_get_vbat(struct sm570x_fuelgauge *fg, int *val)
{
    u8 buf[2];
    int ret, raw, vbat;

    ret = regmap_bulk_read(fg->regmap, SM570X_REG_VBAT, buf, 2);
    if (ret)
        return ret;

    raw = buf[0] | (buf[1] << 8);
    /* Some VBAT formula */
    vbat = ((raw & 0x3800) >> 11) * 1000;
    vbat += (((raw & 0x07ff) * 1000) / 2048);

    *val = vbat * 1000; /* translating mV to µV */
    return 0;
}

static int sm570x_fuelgauge_get_property(struct power_supply *psy,
                                 enum power_supply_property psp,
                                 union power_supply_propval *val)
{
    struct sm570x_fuelgauge *fg = power_supply_get_drvdata(psy);

    switch (psp) {
    case POWER_SUPPLY_PROP_VOLTAGE_NOW:
        return sm570x_fuelgauge_get_vbat(fg, &val->intval);
    default:
        return -EINVAL;
    }
}

static const enum power_supply_property sm570x_fuelgauge_props[] = {
    POWER_SUPPLY_PROP_VOLTAGE_NOW,
};

static const struct power_supply_desc sm570x_fuelgauge_desc = {
    .name = "sm570x-battery",
    .type = POWER_SUPPLY_TYPE_BATTERY,
    .properties = sm570x_fuelgauge_props,
    .num_properties = ARRAY_SIZE(sm570x_fuelgauge_props),
    .get_property = sm570x_fuelgauge_get_property,
};

static int sm570x_fuelgauge_probe(struct platform_device *pdev)
{
    struct sm570x_fuelgauge *fg;
    struct power_supply_config psy_cfg = {};

    fg = devm_kzalloc(&pdev->dev, sizeof(*fg), GFP_KERNEL);
    if (!fg)
        return -ENOMEM;

    fg->dev = &pdev->dev;
    fg->regmap = dev_get_regmap(pdev->dev.parent, NULL);
    if (!fg->regmap)
        return -EINVAL;

    psy_cfg.drv_data = fg;
    fg->psy = devm_power_supply_register(&pdev->dev, &sm570x_fuelgauge_desc, &psy_cfg);

    return PTR_ERR_OR_ZERO(fg->psy);
}

static const struct of_device_id sm570x_fuelgauge_of_match[] = {
    { .compatible = "samsung,sm570x-fuelgauge" },
    { }
};
MODULE_DEVICE_TABLE(of, sm570x_fuelgauge_of_match);

static struct platform_driver sm570x_fuelgauge_driver = {
    .driver = {
        .name = "sm570x-fuelgauge",
        .of_match_table = sm570x_fuelgauge_of_match,
    },
    .probe = sm570x_fuelgauge_probe,
};
module_platform_driver(sm570x_fuelgauge_driver);

MODULE_DESCRIPTION("SM570x Fuel Gauge Driver");
MODULE_LICENSE("GPL");
