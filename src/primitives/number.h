#ifndef KERNEL_NUMBER_H
#define KERNEL_NUMBER_H

#include <lime/api.h>

LimeValue kernel_number_add(LimeStack stack);

LimeValue kernel_number_subtract(LimeStack stack);

LimeValue kernel_number_multiply(LimeStack stack);

LimeValue kernel_number_divide(LimeStack stack);

LimeValue kernel_number_remainder(LimeStack stack);

LimeValue kernel_number_less_than(LimeStack stack);

LimeValue kernel_number_less_equal(LimeStack stack);

LimeValue kernel_number_greater_than(LimeStack stack);

LimeValue kernel_number_greater_equal(LimeStack stack);

#endif