#ifndef KERNEL_VECTOR_H
#define KERNEL_VECTOR_H

#include <lime/api.h>

LimeValue kernel_vector_empty(LimeStack stack);

LimeValue kernel_vector_fill(LimeStack stack);

LimeValue kernel_vector_length(LimeStack stack);

LimeValue kernel_vector_concat(LimeStack stack);

LimeValue kernel_vector_slice(LimeStack stack);

LimeValue kernel_vector_list(LimeStack stack);

#endif