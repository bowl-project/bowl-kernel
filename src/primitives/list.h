#ifndef KERNEL_LIST_H
#define KERNEL_LIST_H

#include <lime/api.h>

LimeValue kernel_list_length(LimeStack stack);

LimeValue kernel_list_empty(LimeStack stack);

LimeValue kernel_list_push(LimeStack stack);

LimeValue kernel_list_pop(LimeStack stack);

LimeValue kernel_list_concat(LimeStack stack);

LimeValue kernel_list_contains(LimeStack stack);

LimeValue kernel_list_slice(LimeStack stack);

LimeValue kernel_list_reverse(LimeStack stack);

LimeValue kernel_list_vector(LimeStack stack);

#endif