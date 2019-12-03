#ifndef KERNEL_H
#define KERNEL_H

#include "../../lime/src/interface/api.h"

LimeValue kernel_type(LimeStack stack);

LimeValue kernel_hash(LimeStack stack);

LimeValue kernel_equals(LimeStack stack);

LimeValue kernel_show(LimeStack stack);

LimeValue kernel_throw(LimeStack stack);

LimeValue kernel_length(LimeStack stack);

LimeValue kernel_nil(LimeStack stack);

LimeValue kernel_push(LimeStack stack);

LimeValue kernel_library(LimeStack stack);

LimeValue kernel_native(LimeStack stack);

LimeValue kernel_run(LimeStack stack);

#endif
