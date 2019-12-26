#include "number.h"

static inline LimeValue kernel_number_binary(LimeStack stack, double (*const operation)(double const, double const)) {
    LimeValue a;
    LimeValue b;

    LIME_STACK_POP_VALUE(stack, &b);
    LIME_ASSERT_TYPE(b, LimeNumberValue);

    LIME_STACK_POP_VALUE(stack, &a);
    LIME_ASSERT_TYPE(a, LimeNumberValue);
    
    LIME_TRY(&a, lime_number(stack, operation(a->number.value, b->number.value)));
    LIME_STACK_PUSH_VALUE(stack, a);

    return NULL;
}

static inline double kernel_number_operation_add(double const a, double const b) {
    return a + b;
}

static inline double kernel_number_operation_subtract(double const a, double const b) {
    return a - b;
}

static inline double kernel_number_operation_multiply(double const a, double const b) {
    return a * b;
}

static inline double kernel_number_operation_divide(double const a, double const b) {
    return a / b;
}

static inline double kernel_number_operation_remainder(double const a, double const b) {
    return a - floor(a / b) * b;
}

LimeValue kernel_number_add(LimeStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_add);
}

LimeValue kernel_number_subtract(LimeStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_subtract);
}

LimeValue kernel_number_multiply(LimeStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_multiply);
}

LimeValue kernel_number_divide(LimeStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_divide);
}

LimeValue kernel_number_remainder(LimeStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_remainder);
}
