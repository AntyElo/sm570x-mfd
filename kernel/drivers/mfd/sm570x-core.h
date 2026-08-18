#ifndef __SM570X_CORE_H__
#define __SM570X_CORE_H__

#include <linux/device.h>
#include <linux/regmap.h>

enum sm570x_type {
    SM5703,
    SM5705,
};

/* Общие регистры Fuel Gauge (постоянны для семейства) */
#define SM570X_REG_DEVICE_ID    0x00
#define SM570X_REG_CNTL         0x01
#define SM570X_REG_OCV          0x06
#define SM570X_REG_VBAT         0x07
#define SM570X_REG_CURRENT      0x08

/* Структура данных для разграничения ревизий */
struct sm570x_chip_info {
    enum sm570x_type type;
    const char *name;
    const struct regmap_config *regmap_cfg;
    const struct regmap_irq_chip *irq_chip;
};

struct sm570x {
    struct device *dev;
    struct regmap *regmap;
    const struct sm570x_chip_info *info;
    struct regmap_irq_chip_data *irq_data;
};

#endif /* __SM570X_CORE_H__ */
