#include "list.h"

LimeValue kernel_list_length(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);
    LIME_ASSERT_TYPE(value, LimeListValue);
    LIME_TRY(&value, lime_number(stack, value == NULL ? 0 : value->list.length));
    LIME_STACK_PUSH_VALUE(stack, value);

    return NULL;
}

LimeValue kernel_list_empty(LimeStack stack) {
    LIME_STACK_PUSH_VALUE(stack, NULL);
    return NULL;
}

LimeValue kernel_list_push(LimeStack stack) {
    LimeValue head;
    LimeValue tail;
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &head);
    LIME_STACK_POP_VALUE(stack, &tail);
    LIME_ASSERT_TYPE(tail, LimeListValue);
    LIME_TRY(&value, lime_list(stack, head, tail));
    LIME_STACK_PUSH_VALUE(stack, value);

    return NULL;
}

LimeValue kernel_list_pop(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeListValue);

    if (frame.registers[0] == NULL) {
        return lime_format_exception(&frame, "empty list provided in function '%s'", __FUNCTION__).value;
    }

    LIME_STACK_PUSH_VALUE(&frame, frame.registers[0]->list.tail);
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[0]->list.head);

    return NULL;
}

LimeValue kernel_list_concat(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[1]);
    LIME_ASSERT_TYPE(frame.registers[1], LimeListValue);
    
    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeListValue);

    while (frame.registers[0] != NULL) {
        LIME_TRY(&frame.registers[2], lime_list(&frame, frame.registers[0]->list.head, frame.registers[2]));
        frame.registers[0] = frame.registers[0]->list.tail;
    }

    frame.registers[0] = frame.registers[1];
    while (frame.registers[2] != NULL) {
        LIME_TRY(&frame.registers[0], lime_list(&frame, frame.registers[2]->list.head, frame.registers[0]));
        frame.registers[2] = frame.registers[2]->list.tail;
    }

    LIME_STACK_PUSH_VALUE(&frame, frame.registers[0]);
    return NULL;
}

LimeValue kernel_list_contains(LimeStack stack) {
    LimeValue list;
    LimeValue element;

    LIME_STACK_POP_VALUE(stack, &element);
    LIME_STACK_POP_VALUE(stack, &list);
    LIME_ASSERT_TYPE(list, LimeListValue);

    bool contains = false;
    while (list != NULL) {
        if (lime_value_equals(list->list.head, element)) {
            contains = true;
            break;
        }
        list = list->list.tail;
    }

    LIME_TRY(&element, lime_boolean(stack, contains));
    LIME_STACK_PUSH_VALUE(stack, element);

    return NULL;
}

LimeValue kernel_list_slice(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);
    LimeValue start;
    LimeValue length;

    LIME_STACK_POP_VALUE(&frame, &start);
    LIME_ASSERT_TYPE(start, LimeNumberValue);

    LIME_STACK_POP_VALUE(&frame, &length);
    LIME_ASSERT_TYPE(length, LimeNumberValue);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeListValue);

    const s64 start_index = (s64) start->number.value;
    const s64 slice_length = (s64) length->number.value;
    const u64 list_length = frame.registers[0] == NULL ? 0 : frame.registers[0]->list.length;

    if (slice_length < 0) {
        return lime_format_exception(&frame, "length must be positive in function '%s' (%" PRId64 " was given)", __FUNCTION__, slice_length).value;
    }

    if (start_index < 0 || start_index > list_length) {
        return lime_format_exception(&frame, "index out of bounds in function '%s' (expected index to be positive and not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, list_length, start_index).value;
    }

    if (start_index + slice_length > list_length) {
        return lime_format_exception(&frame, "length exceeds string bounds in function '%s' (expected length to be not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, list_length - start_index, slice_length).value;
    }

    // skip the start
    u64 index = 0;
    while (index < start_index) {
        frame.registers[0] = frame.registers[0]->list.tail;
        ++index;
    }

    if (start_index + slice_length != list_length) {
        // fetch the slice
        frame.registers[1] = NULL;
        while (index < start_index + slice_length) {
            LIME_TRY(&frame.registers[1], lime_list(&frame, frame.registers[0]->list.head, frame.registers[1]));
            frame.registers[0] = frame.registers[0]->list.tail;
            ++index;
        }

        // reverse the result
        frame.registers[0] = NULL;
        while (frame.registers[1] != NULL) {
            LIME_TRY(&frame.registers[0], lime_list(&frame, frame.registers[1]->list.head, frame.registers[0]));
            frame.registers[1] = frame.registers[1]->list.tail;
        }
    }

    LIME_STACK_PUSH_VALUE(&frame, frame.registers[0]);

    return NULL;
}

LimeValue kernel_list_reverse(LimeStack stack) {
    LimeValue list;

    LIME_STACK_POP_VALUE(stack, &list);
    LIME_ASSERT_TYPE(list, LimeListValue);

    LIME_TRY(&list, lime_list_reverse(stack, list));
    LIME_STACK_PUSH_VALUE(stack, list);

    return NULL;
}

LimeValue kernel_list_vector(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_ASSERT_TYPE(frame.registers[0], LimeListValue);

    const u64 length = frame.registers[0] == NULL ? 0 : frame.registers[0]->list.length;
    LIME_TRY(&frame.registers[1], lime_allocate(&frame, LimeVectorValue, length * sizeof(LimeValue)));
    frame.registers[1]->vector.length = length;

    register u64 index = 0;
    while (frame.registers[0] != NULL) {
        frame.registers[1]->vector.elements[index++] = frame.registers[0]->list.head;
        frame.registers[0] = frame.registers[0]->list.tail;
    }

    LIME_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}
