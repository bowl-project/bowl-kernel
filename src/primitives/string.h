#ifndef KERNEL_STRING_H
#define KERNEL_STRING_H

#include <bowl/api.h>

BowlValue kernel_string_length(BowlStack stack);

BowlValue kernel_string_concat(BowlStack stack);

BowlValue kernel_string_slice(BowlStack stack);

BowlValue kernel_string_number(BowlStack stack);

BowlValue kernel_string_boolean(BowlStack stack);

BowlValue kernel_string_symbol(BowlStack stack);

#endif