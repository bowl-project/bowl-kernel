#include "exception.h"

BowlValue kernel_exception(BowlStack stack) {
    BowlValue value;

    BOWL_STACK_POP_VALUE(stack, &value);
    BOWL_TRY(&value, bowl_exception(stack, NULL, value));
    BOWL_STACK_PUSH_VALUE(stack, value);

    return NULL;
}
