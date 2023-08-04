#ifndef KERNEL_STRING_H
#define KERNEL_STRING_H

#include <bowl/api.h>
#include <bowl/unicode.h>

BowlValue kernel_string_symbol(BowlStack stack);

BowlValue kernel_string_length(BowlStack stack);

BowlValue kernel_string_concat(BowlStack stack);

BowlValue kernel_string_slice(BowlStack stack);

BowlValue kernel_string_char(BowlStack stack);

#endif