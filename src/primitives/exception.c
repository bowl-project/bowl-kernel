#include "exception.h"

LimeValue kernel_exception(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);
    LIME_TRY(&value, lime_exception(stack, NULL, value));
    LIME_STACK_PUSH_VALUE(stack, value);

    return NULL;
}
