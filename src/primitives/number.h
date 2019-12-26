#ifndef KERNEL_NUMBER_H
#define KERNEL_NUMBER_H

#include <lime/api.h>

LimeValue kernel_number_add(LimeStack stack);

LimeValue kernel_number_subtract(LimeStack stack);

LimeValue kernel_number_multiply(LimeStack stack);

LimeValue kernel_number_divide(LimeStack stack);

LimeValue kernel_number_remainder(LimeStack stack);

#endif