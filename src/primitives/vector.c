#include "vector.h"

LimeValue kernel_vector_empty(LimeStack stack) {
    LimeValue vector;
 
    LIME_TRY(&vector, lime_vector(stack, NULL, 0));
    LIME_STACK_PUSH_VALUE(stack, vector);

    return NULL;
}

LimeValue kernel_vector_fill(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[1]);
    LIME_ASSERT_TYPE(frame.registers[1], LimeNumberValue);
    const s64 number = (s64) frame.registers[1]->number.value;

    if (number < 0) {
        return lime_format_exception(&frame, "illegal length %" PRId64 " in function '%s'", number, __FUNCTION__).value;
    }

    frame.registers[1] = NULL;
    LIME_TRY(&frame.registers[0], lime_vector(&frame, frame.registers[0], (u64) number));
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[0]);

    return NULL;
}

LimeValue kernel_vector_length(LimeStack stack) {
    LimeValue vector;

    LIME_STACK_POP_VALUE(stack, &vector);
    LIME_ASSERT_TYPE(vector, LimeVectorValue);

    LIME_TRY(&vector, lime_number(stack, vector->vector.length));
    LIME_STACK_PUSH_VALUE(stack, vector);

    return NULL;
}

LimeValue kernel_vector_concat(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[1]);
    LIME_ASSERT_TYPE(frame.registers[1], LimeVectorValue);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeVectorValue);

    const u64 a_length = frame.registers[0]->vector.length;
    const u64 b_length = frame.registers[1]->vector.length;
    LIME_TRY(&frame.registers[2], lime_allocate(&frame, LimeVectorValue, a_length + b_length));
    frame.registers[2]->vector.length = a_length + b_length;

    memcpy(&frame.registers[2]->vector.elements[0], &frame.registers[0]->vector.elements[0], a_length * sizeof(LimeValue));
    memcpy(&frame.registers[2]->vector.elements[a_length], &frame.registers[1]->vector.elements[0], b_length * sizeof(LimeValue));

    return NULL;
}

LimeValue kernel_vector_slice(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);
    LimeValue start;
    LimeValue length;

    LIME_STACK_POP_VALUE(&frame, &start);
    LIME_ASSERT_TYPE(start, LimeNumberValue);

    LIME_STACK_POP_VALUE(&frame, &length);
    LIME_ASSERT_TYPE(length, LimeNumberValue);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeVectorValue);

    const s64 start_index = (s64) start->number.value;
    const s64 slice_length = (s64) length->number.value;

    if (slice_length < 0) {
        return lime_format_exception(&frame, "length must be positive in function '%s' (%" PRId64 " was given)", __FUNCTION__, slice_length).value;
    }

    if (start_index < 0 || start_index > frame.registers[0]->vector.length) {
        return lime_format_exception(&frame, "index out of bounds in function '%s' (expected index to be positive and not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, frame.registers[0]->vector.length, start_index).value;
    }

    if (start_index + slice_length > frame.registers[0]->vector.length) {
        return lime_format_exception(&frame, "length exceeds string bounds in function '%s' (expected length to be not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, frame.registers[0]->vector.length - start_index, slice_length).value;
    }

    LIME_TRY(&frame.registers[1], lime_allocate(&frame, LimeVectorValue, slice_length * sizeof(LimeValue)));
    frame.registers[1]->vector.length = slice_length;
    memcpy(&frame.registers[1]->vector.elements[0], &frame.registers[0]->vector.elements[start_index], slice_length * sizeof(LimeValue));
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

LimeValue kernel_vector_list(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeVectorValue);

    for (register u64 i = frame.registers[0]->vector.length; i > 0; --i) {
        LIME_TRY(&frame.registers[1], lime_list(&frame, frame.registers[0]->vector.elements[i - 1], frame.registers[1]));
    }

    LIME_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}
