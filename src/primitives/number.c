#include "number.h"

static inline BowlValue kernel_number_comparison(BowlStack stack, bool (*const comparison)(double const, double const)) {
    BowlValue a;
    BowlValue b;

    BOWL_STACK_POP_VALUE(stack, &b);
    BOWL_ASSERT_TYPE(b, BowlNumberValue);

    BOWL_STACK_POP_VALUE(stack, &a);
    BOWL_ASSERT_TYPE(a, BowlNumberValue);
    
    BOWL_TRY(&a, bowl_boolean(stack, comparison(a->number.value, b->number.value)));
    BOWL_STACK_PUSH_VALUE(stack, a);

    return NULL;
}

static inline BowlValue kernel_number_binary(BowlStack stack, double (*const operation)(double const, double const)) {
    BowlValue a;
    BowlValue b;

    BOWL_STACK_POP_VALUE(stack, &b);
    BOWL_ASSERT_TYPE(b, BowlNumberValue);

    BOWL_STACK_POP_VALUE(stack, &a);
    BOWL_ASSERT_TYPE(a, BowlNumberValue);
    
    BOWL_TRY(&a, bowl_number(stack, operation(a->number.value, b->number.value)));
    BOWL_STACK_PUSH_VALUE(stack, a);

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

static inline bool kernel_number_comparison_less_than(double const a, double const b) {
    return a < b;
}

static inline bool kernel_number_comparison_less_equal(double const a, double const b) {
    return a <= b;
}

static inline bool kernel_number_comparison_greater_than(double const a, double const b) {
    return a > b;
}

static inline bool kernel_number_comparison_greater_equal(double const a, double const b) {
    return a >= b;
}

BowlValue kernel_number_add(BowlStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_add);
}

BowlValue kernel_number_subtract(BowlStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_subtract);
}

BowlValue kernel_number_multiply(BowlStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_multiply);
}

BowlValue kernel_number_divide(BowlStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_divide);
}

BowlValue kernel_number_remainder(BowlStack stack) {
    return kernel_number_binary(stack, kernel_number_operation_remainder);
}

BowlValue kernel_number_less_than(BowlStack stack) {
    return kernel_number_comparison(stack, kernel_number_comparison_less_than);
}

BowlValue kernel_number_less_equal(BowlStack stack) {
    return kernel_number_comparison(stack, kernel_number_comparison_less_equal);
}

BowlValue kernel_number_greater_than(BowlStack stack) {
    return kernel_number_comparison(stack, kernel_number_comparison_greater_than);
}

BowlValue kernel_number_greater_equal(BowlStack stack) {
    return kernel_number_comparison(stack, kernel_number_comparison_greater_equal);
}
