#include "map.h"

LimeValue kernel_map_length(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);
    LIME_ASSERT_TYPE(value, LimeMapValue);

    LIME_TRY(&value, lime_number(stack, value->map.length));
    LIME_STACK_PUSH_VALUE(stack, value);

    return NULL;
}

LimeValue kernel_map_empty(LimeStack stack) {
    LimeValue value;
    LIME_TRY(&value, lime_map(stack, 16));
    LIME_STACK_PUSH_VALUE(stack, value);
    return NULL;
}

LimeValue kernel_map_put(LimeStack stack) {
    LimeValue value;
    LimeValue key;
    LimeValue map;

    LIME_STACK_POP_VALUE(stack, &value);
    LIME_STACK_POP_VALUE(stack, &key);
    LIME_STACK_POP_VALUE(stack, &map);
    LIME_ASSERT_TYPE(map, LimeMapValue);

    LIME_TRY(&map, lime_map_put(stack, map, key, value));
    LIME_STACK_PUSH_VALUE(stack, map);

    return NULL;
}

LimeValue kernel_map_get(LimeStack stack) {
    LimeValue value;
    LimeValue key;
    LimeValue map;

    LIME_STACK_POP_VALUE(stack, &value);
    LIME_STACK_POP_VALUE(stack, &key);
    LIME_STACK_POP_VALUE(stack, &map);
    LIME_ASSERT_TYPE(map, LimeMapValue);

    value = lime_map_get_or_else(map, key, value);
    LIME_STACK_PUSH_VALUE(stack, value);

    return NULL;
}

LimeValue kernel_map_del(LimeStack stack) {
    LimeValue key;
    LimeValue map;

    LIME_STACK_POP_VALUE(stack, &key);
    LIME_STACK_POP_VALUE(stack, &map);
    LIME_ASSERT_TYPE(map, LimeMapValue);

    LIME_TRY(&map, lime_map_delete(stack, map, key));
    LIME_STACK_PUSH_VALUE(stack, map);

    return NULL;
}

LimeValue kernel_map_merge(LimeStack stack) {
    LimeValue a;
    LimeValue b;

    LIME_STACK_POP_VALUE(stack, &b);
    LIME_ASSERT_TYPE(b, LimeMapValue);

    LIME_STACK_POP_VALUE(stack, &a);
    LIME_ASSERT_TYPE(a, LimeMapValue);

    LIME_TRY(&a, lime_map_merge(stack, a, b));
    LIME_STACK_PUSH_VALUE(stack, a);

    return NULL;
}

LimeValue kernel_map_contains(LimeStack stack) {
    LimeValue key;
    LimeValue map;

    LIME_STACK_POP_VALUE(stack, &key);
    LIME_STACK_POP_VALUE(stack, &map);
    LIME_ASSERT_TYPE(map, LimeMapValue);

    const bool contains = lime_map_get_or_else(map, key, lime_sentinel_value) != lime_sentinel_value;
    LIME_TRY(&key, lime_boolean(stack, contains));
    LIME_STACK_PUSH_VALUE(stack, key);

    return NULL;
}
