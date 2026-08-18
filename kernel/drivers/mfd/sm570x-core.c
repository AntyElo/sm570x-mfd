#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/mfd/core.h>
#include "sm570x-core.h"

static const struct mfd_cell sm570x_cells[] = {
    {
        .name = "sm570x-fuelgauge",
        .of_compatible = "samsung,sm570x-fuelgauge",
    },
    {
        .name = "sm570x-charger",
        .of_compatible = "samsung,sm570x-charger",
    },
};

static const struct regmap_config sm5705_regmap_config = {
    .reg_bits = 8,
    .val_bits = 8,
    .max_register = 0xFF,
};

static const struct sm570x_chip_info sm5703_info = {
    .type = SM5703,
    .name = "SM5703",
    .regmap_cfg = &sm5705_regmap_config,
};

static const struct sm570x_chip_info sm5705_info = {
    .type = SM5705,
    .name = "SM5705",
    .regmap_cfg = &sm5705_regmap_config,
};

static int sm570x_i2c_probe(struct i2c_client *client)
{
    struct sm570x *sm570x;
    const struct sm570x_chip_info *info;
    int ret;

    info = device_get_match_data(&client->dev);
    if (!info)
        return -ENODEV;

    sm570x = devm_kzalloc(&client->dev, sizeof(*sm570x), GFP_KERNEL);
    if (!sm570x)
        return -ENOMEM;

    sm570x->dev = &client->dev;
    sm570x->info = info;
    i2c_set_clientdata(client, sm570x);

    sm570x->regmap = devm_regmap_init_i2c(client, info->regmap_cfg);
    if (IS_ERR(sm570x->regmap))
        return PTR_ERR(sm570x->regmap);

    /* Инициализация дочерних устройств */
    ret = devm_mfd_add_devices(sm570x->dev, PLATFORM_DEVID_AUTO,
                              sm570x_cells, ARRAY_SIZE(sm570x_cells),
                              NULL, 0, NULL);
    return ret;
}

static const struct of_device_id sm570x_of_match[] = {
    { .compatible = "samsung,sm5703-mfd", .data = &sm5703_info },
    { .compatible = "samsung,sm5705-mfd", .data = &sm5705_info },
    { }
};
MODULE_DEVICE_TABLE(of, sm570x_of_match);

static struct i2c_driver sm570x_driver = {
    .driver = {
        .name = "sm570x",
        .of_match_table = sm570x_of_match,
    },
    .probe = sm570x_i2c_probe,
};
module_i2c_driver(sm570x_driver);

MODULE_AUTHOR("Reference Driver Implementation");
MODULE_DESCRIPTION("Silicon Mitus SM570x Core MFD Driver");
MODULE_LICENSE("GPL");
