#ifndef KERNEL_VECTOR_H
#define KERNEL_VECTOR_H

#include <bowl/api.h>

BowlValue kernel_vector_empty(BowlStack stack);

BowlValue kernel_vector_fill(BowlStack stack);

BowlValue kernel_vector_length(BowlStack stack);

BowlValue kernel_vector_concat(BowlStack stack);

BowlValue kernel_vector_slice(BowlStack stack);

BowlValue kernel_vector_list(BowlStack stack);

BowlValue kernel_vector_reverse(BowlStack stack);

#endif