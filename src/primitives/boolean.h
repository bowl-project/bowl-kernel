#ifndef KERNEL_BOOLEAN_H
#define KERNEL_BOOLEAN_H

#include <lime/api.h>

LimeValue kernel_boolean_and(LimeStack stack);

LimeValue kernel_boolean_or(LimeStack stack);

LimeValue kernel_boolean_xor(LimeStack stack);

LimeValue kernel_boolean_not(LimeStack stack);

#endif