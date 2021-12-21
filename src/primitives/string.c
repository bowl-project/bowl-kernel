#include "string.h"

BowlValue kernel_string_length(BowlStack stack) {
    BowlValue string;

    BOWL_STACK_POP_VALUE(stack, &string);
    BOWL_ASSERT_TYPE(string, BowlStringValue);

    BOWL_TRY(&string, bowl_number(stack, string->string.length));
    BOWL_STACK_PUSH_VALUE(stack, string);

    return NULL;
}

BowlValue kernel_string_concat(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[1]);
    BOWL_ASSERT_TYPE(frame.registers[1], BowlStringValue);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlStringValue);

    const u64 length = frame.registers[0]->string.length + frame.registers[1]->string.length;
    BOWL_TRY(&frame.registers[2], bowl_allocate(&frame, BowlStringValue, length));
    frame.registers[2]->string.length = length;
    memcpy(frame.registers[2]->string.bytes, frame.registers[0]->string.bytes, frame.registers[0]->string.length);
    memcpy(&frame.registers[2]->string.bytes[frame.registers[0]->string.length], frame.registers[1]->string.bytes, frame.registers[1]->string.length);

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

    BOWL_TRY(&frame.registers[1], bowl_allocate(&frame, BowlStringValue, slice_length));
    frame.registers[1]->string.length = slice_length;
    memcpy(frame.registers[1]->string.bytes, &frame.registers[0]->string.bytes[start_index], slice_length);
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

BowlValue kernel_string_number(BowlStack stack) {
    BowlValue string;

    BOWL_STACK_POP_VALUE(stack, &string);
    BOWL_ASSERT_TYPE(string, BowlStringValue);

    char content[512];

    if (string->string.length > sizeof(content) / sizeof(content[0])) {
        // no number is larger than the size of the buffer, i.e. the string cannot be well-formed
        return bowl_format_exception(stack, "illegal format in function '%s' (expected numeric string)", __FUNCTION__).value;
    }

    memcpy(&content[0], &string->string.bytes[0], string->string.length);

    char *const start = &content[0];
    char *end;

    const double result = strtod(start, &end);
    if (end - start != string->string.length) {
        return bowl_format_exception(stack, "illegal format in function '%s' (expected numeric string)", __FUNCTION__).value;
    }

    BOWL_TRY(&string, bowl_number(stack, result));
    BOWL_STACK_PUSH_VALUE(stack, string);

    return NULL;
}

BowlValue kernel_string_boolean(BowlStack stack) {
    BowlValue string;

    BOWL_STACK_POP_VALUE(stack, &string);
    BOWL_ASSERT_TYPE(string, BowlStringValue);

    bool value;
    if (string->string.length == sizeof("true") - 1 && memcmp(string->string.bytes, "true", sizeof("true") - 1) == 0) {
        value = true;
    } else if (string->string.length == sizeof("false") - 1 && memcmp(string->string.bytes, "false", sizeof("false") - 1) == 0) {
        value = false;
    } else {
        return bowl_format_exception(stack, "illegal format in function '%s' (expected either 'true' or 'false')", __FUNCTION__).value;
    }

    BOWL_TRY(&string, bowl_boolean(stack, value));
    BOWL_STACK_PUSH_VALUE(stack, string);

    return NULL;
}

BowlValue kernel_string_symbol(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlStringValue);

    const u64 length = frame.registers[0]->string.length;
    u8 *const bytes = &frame.registers[0]->string.bytes[0];
    for (u64 i = 0; i < length; ++i) {
        if (isspace(bytes[i])) {
            return bowl_format_exception(&frame, "illegal format in function '%s' (a symbol may not contain whitespace)", __FUNCTION__).value;
        }
    }

    BOWL_TRY(&frame.registers[1], bowl_allocate(&frame, BowlSymbolValue, length));
    frame.registers[1]->symbol.length = length;
    memcpy(frame.registers[1]->symbol.bytes, frame.registers[0]->string.bytes, length);

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}
