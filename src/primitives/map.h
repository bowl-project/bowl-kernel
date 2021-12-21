#ifndef KERNEL_MAP_H
#define KERNEL_MAP_H

#include <bowl/api.h>

BowlValue kernel_map_length(BowlStack stack);

BowlValue kernel_map_empty(BowlStack stack);

BowlValue kernel_map_put(BowlStack stack);

BowlValue kernel_map_get(BowlStack stack);

BowlValue kernel_map_del(BowlStack stack);

BowlValue kernel_map_merge(BowlStack stack);

BowlValue kernel_map_contains(BowlStack stack);

#endif