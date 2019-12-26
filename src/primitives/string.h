#ifndef KERNEL_STRING_H
#define KERNEL_STRING_H

#include <lime/api.h>

LimeValue kernel_string_length(LimeStack stack);

LimeValue kernel_string_concat(LimeStack stack);

LimeValue kernel_string_slice(LimeStack stack);

LimeValue kernel_string_number(LimeStack stack);

LimeValue kernel_string_boolean(LimeStack stack);

LimeValue kernel_string_symbol(LimeStack stack);

#endif