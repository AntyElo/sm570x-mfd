#include <linux/module.h>
#include <linux/regmap.h>
#include "sm570x-core.h"

/* Карта прерываний SM570x (смещения и битовые маски) */
static const struct regmap_irq sm5705_irqs[] = {
    [SM570X_IRQ_FG_LOW_BAT] = { .reg_offset = 0, .mask = BIT(0) },
    [SM570X_IRQ_CHG_OVP]    = { .reg_offset = 0, .mask = BIT(1) },
    [SM570X_IRQ_VBUS_DET]   = { .reg_offset = 1, .mask = BIT(3) },
};

const struct regmap_irq_chip sm5705_irq_chip = {
    .name = "sm570x-irq",
    .main_status = 0x10,        /* Регистр статуса INT1 */
    .num_regs = 2,
    .irqs = sm5705_irqs,
    .num_irqs = ARRAY_SIZE(sm5705_irqs),
    .status_base = 0x10,
    .mask_base = 0x12,          /* Регистр маски INTMASK1 */
    .ack_base = 0x10,
};
