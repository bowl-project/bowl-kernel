#ifndef KERNEL_LIST_H
#define KERNEL_LIST_H

#include <bowl/api.h>

BowlValue kernel_list_length(BowlStack stack);

BowlValue kernel_list_empty(BowlStack stack);

BowlValue kernel_list_push(BowlStack stack);

BowlValue kernel_list_pop(BowlStack stack);

BowlValue kernel_list_concat(BowlStack stack);

BowlValue kernel_list_contains(BowlStack stack);

BowlValue kernel_list_slice(BowlStack stack);

BowlValue kernel_list_reverse(BowlStack stack);

BowlValue kernel_list_vector(BowlStack stack);

#endif