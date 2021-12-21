#include "map.h"

BowlValue kernel_map_length(BowlStack stack) {
    BowlValue value;

    BOWL_STACK_POP_VALUE(stack, &value);
    BOWL_ASSERT_TYPE(value, BowlMapValue);

    BOWL_TRY(&value, bowl_number(stack, value->map.length));
    BOWL_STACK_PUSH_VALUE(stack, value);

    return NULL;
}

BowlValue kernel_map_empty(BowlStack stack) {
    BowlValue value;
    BOWL_TRY(&value, bowl_map(stack, 16));
    BOWL_STACK_PUSH_VALUE(stack, value);
    return NULL;
}

BowlValue kernel_map_put(BowlStack stack) {
    BowlValue value;
    BowlValue key;
    BowlValue map;

    BOWL_STACK_POP_VALUE(stack, &value);
    BOWL_STACK_POP_VALUE(stack, &key);
    BOWL_STACK_POP_VALUE(stack, &map);
    BOWL_ASSERT_TYPE(map, BowlMapValue);

    BOWL_TRY(&map, bowl_map_put(stack, map, key, value));
    BOWL_STACK_PUSH_VALUE(stack, map);

    return NULL;
}

BowlValue kernel_map_get(BowlStack stack) {
    BowlValue value;
    BowlValue key;
    BowlValue map;

    BOWL_STACK_POP_VALUE(stack, &value);
    BOWL_STACK_POP_VALUE(stack, &key);
    BOWL_STACK_POP_VALUE(stack, &map);
    BOWL_ASSERT_TYPE(map, BowlMapValue);

    value = bowl_map_get_or_else(map, key, value);
    BOWL_STACK_PUSH_VALUE(stack, value);

    return NULL;
}

BowlValue kernel_map_del(BowlStack stack) {
    BowlValue key;
    BowlValue map;

    BOWL_STACK_POP_VALUE(stack, &key);
    BOWL_STACK_POP_VALUE(stack, &map);
    BOWL_ASSERT_TYPE(map, BowlMapValue);

    BOWL_TRY(&map, bowl_map_delete(stack, map, key));
    BOWL_STACK_PUSH_VALUE(stack, map);

    return NULL;
}

BowlValue kernel_map_merge(BowlStack stack) {
    BowlValue a;
    BowlValue b;

    BOWL_STACK_POP_VALUE(stack, &b);
    BOWL_ASSERT_TYPE(b, BowlMapValue);

    BOWL_STACK_POP_VALUE(stack, &a);
    BOWL_ASSERT_TYPE(a, BowlMapValue);

    BOWL_TRY(&a, bowl_map_merge(stack, a, b));
    BOWL_STACK_PUSH_VALUE(stack, a);

    return NULL;
}

BowlValue kernel_map_contains(BowlStack stack) {
    BowlValue key;
    BowlValue map;

    BOWL_STACK_POP_VALUE(stack, &key);
    BOWL_STACK_POP_VALUE(stack, &map);
    BOWL_ASSERT_TYPE(map, BowlMapValue);

    const bool contains = bowl_map_get_or_else(map, key, bowl_sentinel_value) != bowl_sentinel_value;
    BOWL_TRY(&key, bowl_boolean(stack, contains));
    BOWL_STACK_PUSH_VALUE(stack, key);

    return NULL;
}
