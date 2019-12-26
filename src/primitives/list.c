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
        return lime_exception(&frame, "empty list provided in function '%s'", __FUNCTION__);
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
