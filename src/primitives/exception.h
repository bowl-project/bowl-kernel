#ifndef KERNEL_EXCEPTION_H
#define KERNEL_EXCEPTION_H

#include <bowl/api.h>

BowlValue kernel_exception(BowlStack stack);

BowlValue kernel_exception_message(BowlStack stack);

#endif