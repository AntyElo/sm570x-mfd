#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/regmap.h>
#include "sm570x-core.h"

struct sm570x_chg {
    struct device *dev;
    struct regmap *regmap;
    struct power_supply *psy;
};

static int sm570x_chg_get_property(struct power_supply *psy,
                                  enum power_supply_property psp,
                                  union power_supply_propval *val)
{
    struct sm570x_chg *chg = power_supply_get_drvdata(psy);
    u32 reg_val;
    int ret;

    switch (psp) {
    case POWER_SUPPLY_PROP_ONLINE:
        /* Чтение статуса VBUS из регистра MFD */
        ret = regmap_read(chg->regmap, 0x0F, &reg_val);
        if (ret)
            return ret;
        val->intval = !!(reg_val & BIT(3));
        return 0;

    case POWER_SUPPLY_PROP_STATUS:
        ret = regmap_read(chg->regmap, 0x0F, &reg_val);
        if (ret)
            return ret;
        if (reg_val & BIT(3))
            val->intval = POWER_SUPPLY_STATUS_CHARGING;
        else
            val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
        return 0;

    default:
        return -EINVAL;
    }
}

static const enum power_supply_property sm570x_chg_props[] = {
    POWER_SUPPLY_PROP_ONLINE,
    POWER_SUPPLY_PROP_STATUS,
};

static const struct power_supply_desc sm570x_chg_desc = {
    .name = "sm570x-charger",
    .type = POWER_SUPPLY_TYPE_USB,
    .properties = sm570x_chg_props,
    .num_properties = ARRAY_SIZE(sm570x_chg_props),
    .get_property = sm570x_chg_get_property,
};

static int sm570x_chg_probe(struct platform_device *pdev)
{
    struct sm570x_chg *chg;
    struct power_supply_config psy_cfg = {};

    chg = devm_kzalloc(&pdev->dev, sizeof(*chg), GFP_KERNEL);
    if (!chg)
        return -ENOMEM;

    chg->dev = &pdev->dev;
    chg->regmap = dev_get_regmap(pdev->dev.parent, NULL);
    if (!chg->regmap)
        return -EINVAL;

    psy_cfg.drv_data = chg;
    chg->psy = devm_power_supply_register(&pdev->dev, &sm570x_chg_desc, &psy_cfg);

    return PTR_ERR_OR_ZERO(chg->psy);
}

static const struct of_device_id sm570x_chg_of_match[] = {
    { .compatible = "samsung,sm570x-charger" },
    { }
};
MODULE_DEVICE_TABLE(of, sm570x_chg_of_match);

static struct platform_driver sm570x_chg_driver = {
    .driver = {
        .name = "sm570x-charger",
        .of_match_table = sm570x_chg_of_match,
    },
    .probe = sm570x_chg_probe,
};
module_platform_driver(sm570x_chg_driver);

MODULE_DESCRIPTION("SM570x Charger Driver");
MODULE_LICENSE("GPL");
