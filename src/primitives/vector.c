#include "vector.h"

BowlValue kernel_vector_empty(BowlStack stack) {
    BowlValue vector;
 
    BOWL_TRY(&vector, bowl_vector(stack, NULL, 0));
    BOWL_STACK_PUSH_VALUE(stack, vector);

    return NULL;
}

BowlValue kernel_vector_fill(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[1]);
    BOWL_ASSERT_TYPE(frame.registers[1], BowlNumberValue);
    const s64 number = (s64) frame.registers[1]->number.value;

    if (number < 0) {
        return bowl_format_exception(&frame, "illegal length %" PRId64 " in function '%s'", number, __FUNCTION__).value;
    }

    frame.registers[1] = NULL;
    BOWL_TRY(&frame.registers[0], bowl_vector(&frame, frame.registers[0], (u64) number));
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]);

    return NULL;
}

BowlValue kernel_vector_length(BowlStack stack) {
    BowlValue vector;

    BOWL_STACK_POP_VALUE(stack, &vector);
    BOWL_ASSERT_TYPE(vector, BowlVectorValue);

    BOWL_TRY(&vector, bowl_number(stack, vector->vector.length));
    BOWL_STACK_PUSH_VALUE(stack, vector);

    return NULL;
}

BowlValue kernel_vector_concat(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[1]);
    BOWL_ASSERT_TYPE(frame.registers[1], BowlVectorValue);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlVectorValue);

    const u64 a_length = frame.registers[0]->vector.length;
    const u64 b_length = frame.registers[1]->vector.length;
    BOWL_TRY(&frame.registers[2], bowl_allocate(&frame, BowlVectorValue, a_length + b_length));
    frame.registers[2]->vector.length = a_length + b_length;

    memcpy(&frame.registers[2]->vector.elements[0], &frame.registers[0]->vector.elements[0], a_length * sizeof(BowlValue));
    memcpy(&frame.registers[2]->vector.elements[a_length], &frame.registers[1]->vector.elements[0], b_length * sizeof(BowlValue));

    return NULL;
}

BowlValue kernel_vector_slice(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);
    BowlValue start;
    BowlValue length;

    BOWL_STACK_POP_VALUE(&frame, &start);
    BOWL_ASSERT_TYPE(start, BowlNumberValue);

    BOWL_STACK_POP_VALUE(&frame, &length);
    BOWL_ASSERT_TYPE(length, BowlNumberValue);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlVectorValue);

    const s64 start_index = (s64) start->number.value;
    const s64 slice_length = (s64) length->number.value;

    if (slice_length < 0) {
        return bowl_format_exception(&frame, "length must be positive in function '%s' (%" PRId64 " was given)", __FUNCTION__, slice_length).value;
    }

    if (start_index < 0 || start_index > frame.registers[0]->vector.length) {
        return bowl_format_exception(&frame, "index out of bounds in function '%s' (expected index to be positive and not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, frame.registers[0]->vector.length, start_index).value;
    }

    if (start_index + slice_length > frame.registers[0]->vector.length) {
        return bowl_format_exception(&frame, "length exceeds string bounds in function '%s' (expected length to be not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, frame.registers[0]->vector.length - start_index, slice_length).value;
    }

    BOWL_TRY(&frame.registers[1], bowl_allocate(&frame, BowlVectorValue, slice_length * sizeof(BowlValue)));
    frame.registers[1]->vector.length = slice_length;
    memcpy(&frame.registers[1]->vector.elements[0], &frame.registers[0]->vector.elements[start_index], slice_length * sizeof(BowlValue));
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

BowlValue kernel_vector_list(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlVectorValue);

    for (register u64 i = frame.registers[0]->vector.length; i > 0; --i) {
        BOWL_TRY(&frame.registers[1], bowl_list(&frame, frame.registers[0]->vector.elements[i - 1], frame.registers[1]));
    }

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

BowlValue kernel_vector_reverse(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlVectorValue);

    const u64 length = frame.registers[0]->vector.length;
    BOWL_TRY(&frame.registers[1], bowl_allocate(&frame, BowlVectorValue, length * sizeof(BowlValue)));

    frame.registers[1]->vector.length = length;
    BowlValue *const src = &frame.registers[0]->vector.elements[0];
    BowlValue *const dst = &frame.registers[1]->vector.elements[0];
    for (register u64 i = 0; i < length; ++i) {
        dst[i] = src[length - i - 1];
    }

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}
