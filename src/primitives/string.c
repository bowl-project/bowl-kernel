#include "string.h"

BowlValue kernel_string_symbol(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlStringValue);

    const u64 length = frame.registers[0]->string.length;
    for (u64 i = 0; i < length; ++i) {
        if (unicode_is_space(frame.registers[0]->string.codepoints[i])) {
            return bowl_format_exception(&frame, "illegal format in function '%s' (whitespace is not allowed inside symbols)", __FUNCTION__).value;
        }
    }

    BOWL_TRY(&frame.registers[1], bowl_allocate(&frame, BowlSymbolValue, length * sizeof(u32)));
    memcpy(&frame.registers[1]->symbol.codepoints[0], &frame.registers[0]->string.codepoints[0], length * sizeof(u32));
    frame.registers[1]->symbol.length = length;
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);
    
    return NULL;
}

BowlValue kernel_string_concat(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[1]);
    BOWL_ASSERT_TYPE(frame.registers[1], BowlStringValue);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlStringValue);

    const u64 length = frame.registers[0]->string.length + frame.registers[1]->string.length;
    BOWL_TRY(&frame.registers[2], bowl_allocate(&frame, BowlStringValue, length * sizeof(u32)));
    frame.registers[2]->string.length = length;

    memcpy(&frame.registers[2]->string.codepoints[0], &frame.registers[0]->string.codepoints[0], frame.registers[0]->string.length * sizeof(u32));
    memcpy(&frame.registers[2]->string.codepoints[frame.registers[0]->string.length], &frame.registers[1]->string.codepoints[0], frame.registers[1]->string.length * sizeof(u32));

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[2]);

    return NULL;
}

BowlValue kernel_string_slice(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);
    BowlValue start;
    BowlValue length;

    BOWL_STACK_POP_VALUE(&frame, &start);
    BOWL_ASSERT_TYPE(start, BowlNumberValue);

    BOWL_STACK_POP_VALUE(&frame, &length);
    BOWL_ASSERT_TYPE(length, BowlNumberValue);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlStringValue);

    const s64 start_index = (s64) start->number.value;
    const s64 slice_length = (s64) length->number.value;

    if (slice_length < 0) {
        return bowl_format_exception(&frame, "length must be positive in function '%s' (%" PRId64 " was given)", __FUNCTION__, slice_length).value;
    }

    if (start_index < 0 || start_index > frame.registers[0]->string.length) {
        return bowl_format_exception(&frame, "index out of bounds in function '%s' (expected index to be positive and not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, frame.registers[0]->string.length, start_index).value;
    }

    if (start_index + slice_length > frame.registers[0]->string.length) {
        return bowl_format_exception(&frame, "length exceeds string bounds in function '%s' (expected length to be not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, frame.registers[0]->string.length - start_index, slice_length).value;
    }

    BOWL_TRY(&frame.registers[1], bowl_allocate(&frame, BowlStringValue, slice_length * sizeof(u32)));
    frame.registers[1]->string.length = slice_length;
    memcpy(&frame.registers[1]->string.codepoints[0], &frame.registers[0]->string.codepoints[start_index], slice_length * sizeof(u32));
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

BowlValue kernel_string_length(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlStringValue);

    BOWL_TRY(&frame.registers[0], bowl_number(&frame, (double) frame.registers[0]->string.length));
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]);

    return NULL;
}

BowlValue kernel_string_char(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[1]);
    BOWL_ASSERT_TYPE(frame.registers[1], BowlNumberValue);
    const s64 index = (s64) frame.registers[1]->number.value;

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlStringValue);
    const u64 length = (u64) frame.registers[0]->string.length;

    if (index < 0 || index >= length) {
        return bowl_format_exception(&frame, "index out of bounds in function '%s' (attempt to access character %" PRId64 " in string of length %" PRIu64 ")", __FUNCTION__, index, length).value;
    }

    BOWL_TRY(&frame.registers[2], bowl_number(&frame, frame.registers[0]->string.codepoints[index]));
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[2]);
    return NULL;
}
