#include "boolean.h"

static inline LimeValue kernel_boolean_binary(LimeStack stack, bool (*const operation)(bool const, bool const)) {
    LimeValue a;
    LimeValue b;

    LIME_STACK_POP_VALUE(stack, &b);
    LIME_ASSERT_TYPE(b, LimeBooleanValue);

    LIME_STACK_POP_VALUE(stack, &a);
    LIME_ASSERT_TYPE(a, LimeBooleanValue);
    
    LIME_TRY(&a, lime_boolean(stack, operation(a->boolean.value, b->boolean.value)));
    LIME_STACK_PUSH_VALUE(stack, a);

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

LimeValue kernel_boolean_and(LimeStack stack) {
    return kernel_boolean_binary(stack, kernel_boolean_operation_and);
}

LimeValue kernel_boolean_or(LimeStack stack) {
    return kernel_boolean_binary(stack, kernel_boolean_operation_or);
}

LimeValue kernel_boolean_xor(LimeStack stack) {
    return kernel_boolean_binary(stack, kernel_boolean_operation_xor);
}

LimeValue kernel_boolean_not(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);
    LIME_ASSERT_TYPE(value, LimeBooleanValue);

    LIME_TRY(&value, lime_boolean(stack, !value->boolean.value));
    LIME_STACK_PUSH_VALUE(stack, value);

    return NULL;
}
