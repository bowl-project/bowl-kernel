#include "boolean.h"

static inline BowlValue kernel_boolean_binary(BowlStack stack, bool (*const operation)(bool const, bool const)) {
    BowlValue a;
    BowlValue b;

    BOWL_STACK_POP_VALUE(stack, &b);
    BOWL_ASSERT_TYPE(b, BowlBooleanValue);

    BOWL_STACK_POP_VALUE(stack, &a);
    BOWL_ASSERT_TYPE(a, BowlBooleanValue);
    
    BOWL_TRY(&a, bowl_boolean(stack, operation(a->boolean.value, b->boolean.value)));
    BOWL_STACK_PUSH_VALUE(stack, a);

    return NULL;
}

static inline bool kernel_boolean_operation_and(bool const a, bool const b) {
    return a & b;
}

static inline bool kernel_boolean_operation_or(bool const a, bool const b) {
    return a | b;
}

static inline bool kernel_boolean_operation_xor(bool const a, bool const b) {
    return a ^ b;
}

BowlValue kernel_boolean_and(BowlStack stack) {
    return kernel_boolean_binary(stack, kernel_boolean_operation_and);
}

BowlValue kernel_boolean_or(BowlStack stack) {
    return kernel_boolean_binary(stack, kernel_boolean_operation_or);
}

BowlValue kernel_boolean_xor(BowlStack stack) {
    return kernel_boolean_binary(stack, kernel_boolean_operation_xor);
}

BowlValue kernel_boolean_not(BowlStack stack) {
    BowlValue value;

    BOWL_STACK_POP_VALUE(stack, &value);
    BOWL_ASSERT_TYPE(value, BowlBooleanValue);

    BOWL_TRY(&value, bowl_boolean(stack, !value->boolean.value));
    BOWL_STACK_PUSH_VALUE(stack, value);

    return NULL;
}

BowlValue kernel_boolean_choose(BowlStack stack) {
    BowlValue a;
    BowlValue b;
    BowlValue boolean;

    BOWL_STACK_POP_VALUE(stack, &b);
    BOWL_STACK_POP_VALUE(stack, &a);
    BOWL_STACK_POP_VALUE(stack, &boolean);
    BOWL_ASSERT_TYPE(boolean, BowlBooleanValue);

    if (boolean->boolean.value) {
        BOWL_STACK_PUSH_VALUE(stack, a);
    } else {
        BOWL_STACK_PUSH_VALUE(stack, b);
    }

    return NULL;
}
