#include "list.h"

BowlValue kernel_list_length(BowlStack stack) {
    BowlValue value;

    BOWL_STACK_POP_VALUE(stack, &value);
    BOWL_ASSERT_TYPE(value, BowlListValue);
    BOWL_TRY(&value, bowl_number(stack, value == NULL ? 0 : value->list.length));
    BOWL_STACK_PUSH_VALUE(stack, value);

    return NULL;
}

BowlValue kernel_list_empty(BowlStack stack) {
    BOWL_STACK_PUSH_VALUE(stack, NULL);
    return NULL;
}

BowlValue kernel_list_push(BowlStack stack) {
    BowlValue head;
    BowlValue tail;
    BowlValue value;

    BOWL_STACK_POP_VALUE(stack, &head);
    BOWL_STACK_POP_VALUE(stack, &tail);
    BOWL_ASSERT_TYPE(tail, BowlListValue);
    BOWL_TRY(&value, bowl_list(stack, head, tail));
    BOWL_STACK_PUSH_VALUE(stack, value);

    return NULL;
}

BowlValue kernel_list_pop(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlListValue);

    if (frame.registers[0] == NULL) {
        return bowl_format_exception(&frame, "empty list provided in function '%s'", __FUNCTION__).value;
    }

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]->list.tail);
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]->list.head);

    return NULL;
}

BowlValue kernel_list_concat(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[1]);
    BOWL_ASSERT_TYPE(frame.registers[1], BowlListValue);
    
    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlListValue);

    while (frame.registers[0] != NULL) {
        BOWL_TRY(&frame.registers[2], bowl_list(&frame, frame.registers[0]->list.head, frame.registers[2]));
        frame.registers[0] = frame.registers[0]->list.tail;
    }

    frame.registers[0] = frame.registers[1];
    while (frame.registers[2] != NULL) {
        BOWL_TRY(&frame.registers[0], bowl_list(&frame, frame.registers[2]->list.head, frame.registers[0]));
        frame.registers[2] = frame.registers[2]->list.tail;
    }

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]);
    return NULL;
}

BowlValue kernel_list_contains(BowlStack stack) {
    BowlValue list;
    BowlValue element;

    BOWL_STACK_POP_VALUE(stack, &element);
    BOWL_STACK_POP_VALUE(stack, &list);
    BOWL_ASSERT_TYPE(list, BowlListValue);

    bool contains = false;
    while (list != NULL) {
        if (bowl_value_equals(list->list.head, element)) {
            contains = true;
            break;
        }
        list = list->list.tail;
    }

    BOWL_TRY(&element, bowl_boolean(stack, contains));
    BOWL_STACK_PUSH_VALUE(stack, element);

    return NULL;
}

BowlValue kernel_list_slice(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);
    BowlValue start;
    BowlValue length;

    BOWL_STACK_POP_VALUE(&frame, &start);
    BOWL_ASSERT_TYPE(start, BowlNumberValue);

    BOWL_STACK_POP_VALUE(&frame, &length);
    BOWL_ASSERT_TYPE(length, BowlNumberValue);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlListValue);

    const s64 start_index = (s64) start->number.value;
    const s64 slice_length = (s64) length->number.value;
    const u64 list_length = frame.registers[0] == NULL ? 0 : frame.registers[0]->list.length;

    if (slice_length < 0) {
        return bowl_format_exception(&frame, "length must be positive in function '%s' (%" PRId64 " was given)", __FUNCTION__, slice_length).value;
    }

    if (start_index < 0 || start_index > list_length) {
        return bowl_format_exception(&frame, "index out of bounds in function '%s' (expected index to be positive and not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, list_length, start_index).value;
    }

    if (start_index + slice_length > list_length) {
        return bowl_format_exception(&frame, "length exceeds string bounds in function '%s' (expected length to be not greater than %" PRId64 " but %" PRId64 " was given)", __FUNCTION__, list_length - start_index, slice_length).value;
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
            BOWL_TRY(&frame.registers[1], bowl_list(&frame, frame.registers[0]->list.head, frame.registers[1]));
            frame.registers[0] = frame.registers[0]->list.tail;
            ++index;
        }

        // reverse the result
        frame.registers[0] = NULL;
        while (frame.registers[1] != NULL) {
            BOWL_TRY(&frame.registers[0], bowl_list(&frame, frame.registers[1]->list.head, frame.registers[0]));
            frame.registers[1] = frame.registers[1]->list.tail;
        }
    }

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]);

    return NULL;
}

BowlValue kernel_list_reverse(BowlStack stack) {
    BowlValue list;

    BOWL_STACK_POP_VALUE(stack, &list);
    BOWL_ASSERT_TYPE(list, BowlListValue);

    BOWL_TRY(&list, bowl_list_reverse(stack, list));
    BOWL_STACK_PUSH_VALUE(stack, list);

    return NULL;
}

BowlValue kernel_list_vector(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_ASSERT_TYPE(frame.registers[0], BowlListValue);

    const u64 length = frame.registers[0] == NULL ? 0 : frame.registers[0]->list.length;
    BOWL_TRY(&frame.registers[1], bowl_allocate(&frame, BowlVectorValue, length * sizeof(BowlValue)));
    frame.registers[1]->vector.length = length;

    register u64 index = 0;
    while (frame.registers[0] != NULL) {
        frame.registers[1]->vector.elements[index++] = frame.registers[0]->list.head;
        frame.registers[0] = frame.registers[0]->list.tail;
    }

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}
