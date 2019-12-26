#ifndef KERNEL_MAP_H
#define KERNEL_MAP_H

#include <lime/api.h>

LimeValue kernel_map_length(LimeStack stack);

LimeValue kernel_map_empty(LimeStack stack);

LimeValue kernel_map_put(LimeStack stack);

LimeValue kernel_map_get(LimeStack stack);

LimeValue kernel_map_del(LimeStack stack);

LimeValue kernel_map_merge(LimeStack stack);

LimeValue kernel_map_contains(LimeStack stack);

#endif