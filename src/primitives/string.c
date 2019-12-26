#include "string.h"

LimeValue kernel_string_length(LimeStack stack) {
    LimeValue string;

    LIME_STACK_POP_VALUE(stack, &string);
    LIME_ASSERT_TYPE(string, LimeStringValue);

    LIME_TRY(&string, lime_number(stack, string->string.length));
    LIME_STACK_PUSH_VALUE(stack, string);

    return NULL;
}

LimeValue kernel_string_concat(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[1]);
    LIME_ASSERT_TYPE(frame.registers[1], LimeStringValue);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeStringValue);

    const u64 length = frame.registers[0]->string.length + frame.registers[1]->string.length;
    LIME_TRY(&frame.registers[2], lime_allocate(&frame, LimeStringValue, length));
    frame.registers[2]->string.length = length;
    memcpy(frame.registers[2]->string.bytes, frame.registers[0]->string.bytes, frame.registers[0]->string.length);
    memcpy(&frame.registers[2]->string.bytes[frame.registers[0]->string.length], frame.registers[1]->string.bytes, frame.registers[1]->string.length);

    LIME_STACK_PUSH_VALUE(&frame, frame.registers[2]);

    return NULL;
}

LimeValue kernel_string_slice(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);
    LimeValue start;
    LimeValue length;

    LIME_STACK_POP_VALUE(&frame, &length);
    LIME_ASSERT_TYPE(length, LimeNumberValue);

    LIME_STACK_POP_VALUE(&frame, &start);
    LIME_ASSERT_TYPE(start, LimeNumberValue);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeStringValue);

    const s64 start_index = (s64) start->number.value;
    const s64 slice_length = (s64) length->number.value;

    if (slice_length < 0) {
        return lime_exception(&frame, "length must be positive in function '%s' (%" PRId64 " was given)", __FUNCTION__, slice_length);
    }

    if (start_index < 0 || start_index > frame.registers[0]->string.length) {
        return lime_exception(&frame, "index out of bounds in function '%s' (expected index to be positive and smaller than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, frame.registers[0]->string.length, start_index);
    }

    if (start_index + slice_length > frame.registers[0]->string.length) {
        return lime_exception(&frame, "length exceeds string bounds in function '%s' (expected length to be not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, frame.registers[0]->string.length - start_index, slice_length);
    }

    LIME_TRY(&frame.registers[1], lime_allocate(&frame, LimeStringValue, slice_length));
    frame.registers[1]->string.length = slice_length;
    memcpy(frame.registers[1]->string.bytes, &frame.registers[0]->string.bytes[start_index], slice_length);
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

LimeValue kernel_string_number(LimeStack stack) {
    LimeValue string;

    LIME_STACK_POP_VALUE(stack, &string);
    LIME_ASSERT_TYPE(string, LimeStringValue);

    char content[512];

    if (string->string.length > sizeof(content) / sizeof(content[0])) {
        // no number is larger than the size of the buffer, i.e. the string cannot be well-formed
        return lime_exception(stack, "illegal format in function '%s' (expected numeric string)", __FUNCTION__);
    }

    memcpy(&content[0], &string->string.bytes[0], string->string.length);

    char *const start = &content[0];
    char *end;

    const double result = strtod(start, &end);
    if (end - start != string->string.length) {
        return lime_exception(stack, "illegal format in function '%s' (expected numeric string)", __FUNCTION__);
    }

    LIME_TRY(&string, lime_number(stack, result));
    LIME_STACK_PUSH_VALUE(stack, string);

    return NULL;
}

LimeValue kernel_string_boolean(LimeStack stack) {
    LimeValue string;

    LIME_STACK_POP_VALUE(stack, &string);
    LIME_ASSERT_TYPE(string, LimeStringValue);

    bool value;
    if (string->string.length == sizeof("true") - 1 && memcmp(string->string.bytes, "true", sizeof("true") - 1) == 0) {
        value = true;
    } else if (string->string.length == sizeof("false") - 1 && memcmp(string->string.bytes, "false", sizeof("false") - 1) == 0) {
        value = false;
    } else {
        return lime_exception(stack, "illegal format in function '%s' (expected either 'true' or 'false')", __FUNCTION__);
    }

    LIME_TRY(&string, lime_boolean(stack, value));
    LIME_STACK_PUSH_VALUE(stack, string);

    return NULL;
}

LimeValue kernel_string_symbol(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeStringValue);

    const u64 length = frame.registers[0]->string.length;
    u8 *const bytes = &frame.registers[0]->string.bytes[0];
    for (u64 i = 0; i < length; ++i) {
        if (isspace(bytes[i])) {
            return lime_exception(&frame, "illegal format in function '%s' (a symbol may not contain whitespace)", __FUNCTION__);
        }
    }

    LIME_TRY(&frame.registers[1], lime_allocate(&frame, LimeSymbolValue, length));
    frame.registers[1]->symbol.length = length;
    memcpy(frame.registers[1]->symbol.bytes, frame.registers[0]->string.bytes, length);

    LIME_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}
