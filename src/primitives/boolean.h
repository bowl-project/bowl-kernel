#ifndef KERNEL_BOOLEAN_H
#define KERNEL_BOOLEAN_H

#include <bowl/api.h>

BowlValue kernel_boolean_and(BowlStack stack);

BowlValue kernel_boolean_or(BowlStack stack);

BowlValue kernel_boolean_xor(BowlStack stack);

BowlValue kernel_boolean_not(BowlStack stack);

BowlValue kernel_boolean_choose(BowlStack stack);

#endif