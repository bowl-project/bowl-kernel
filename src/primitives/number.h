#ifndef KERNEL_NUMBER_H
#define KERNEL_NUMBER_H

#include <bowl/api.h>

BowlValue kernel_number_add(BowlStack stack);

BowlValue kernel_number_subtract(BowlStack stack);

BowlValue kernel_number_multiply(BowlStack stack);

BowlValue kernel_number_divide(BowlStack stack);

BowlValue kernel_number_remainder(BowlStack stack);

BowlValue kernel_number_less_than(BowlStack stack);

BowlValue kernel_number_less_equal(BowlStack stack);

BowlValue kernel_number_greater_than(BowlStack stack);

BowlValue kernel_number_greater_equal(BowlStack stack);

#endif