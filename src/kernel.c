#include "kernel.h"

static KernelFunctionEntry kernel_functions[] = {
    { .name = "run", .function = kernel_run },
    { .name = "dup", .function = kernel_dup },
    { .name = "type", .function = kernel_type },
    { .name = "hash", .function = kernel_hash },
    { .name = "equals", .function = kernel_equals },
    { .name = "show", .function = kernel_show },
    { .name = "throw", .function = kernel_throw },
    { .name = "length", .function = kernel_length },
    { .name = "+", .function = kernel_add },
    { .name = "contains", .function = kernel_contains },
    { .name = "nim", .function = kernel_nim },
    { .name = "put", .function = kernel_put },
    { .name = "get", .function = kernel_get },
    { .name = "del", .function = kernel_del },
    { .name = "nil", .function = kernel_nil },
    { .name = "push", .function = kernel_push },
    { .name = "pop", .function = kernel_pop },
    { .name = "library", .function = kernel_library },
    { .name = "native", .function = kernel_native }
};

LimeValue lime_module_initialize(LimeStack stack, LimeValue library) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, library, NULL, NULL);

    for (u64 i = 0; i < sizeof(kernel_functions) / sizeof(kernel_functions[0]); ++i) {
        KernelFunctionEntry *const entry = &kernel_functions[i];
        const LimeValue exception = lime_register_function(&frame, entry->name, frame.registers[0], entry->function);
        if (exception != NULL) {
            return exception;
        }
    }

    return NULL;    
}

LimeValue lime_module_finalize(LimeStack stack, LimeValue library) {
    return NULL;
}

LimeValue kernel_add(LimeStack stack) {
    LimeStackFrame temporary = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(&temporary, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[1]);
    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);

    const LimeValueType a_type = frame.registers[0] == NULL ? LimeListValue : frame.registers[0]->type;
    const LimeValueType b_type = frame.registers[1] == NULL ? LimeListValue : frame.registers[1]->type;

    if (a_type != b_type) {
        return lime_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected '%s')", lime_value_type(frame.registers[1]), __FUNCTION__, lime_value_type(frame.registers[0]));
    }
    
    LimeResult result;
    switch (a_type) {
        case LimeListValue:
            // list concatenation

            frame.registers[2] = NULL;
            while (frame.registers[0] != NULL) {
                result = lime_list(&frame, frame.registers[0]->list.head, frame.registers[2]);

                if (result.failure) {
                    return result.exception;
                } else {
                    frame.registers[2] = result.value;
                }

                frame.registers[0] = frame.registers[0]->list.tail;
            }

            frame.registers[0] = frame.registers[1];
            while (frame.registers[2] != NULL) {
                result = lime_list(&frame, frame.registers[2]->list.head, frame.registers[0]);

                if (result.failure) {
                    return result.exception;
                } else {
                    frame.registers[0] = result.value;
                }

                frame.registers[2] = frame.registers[2]->list.tail;
            }

            result.value = frame.registers[0];
            break;
        case LimeMapValue:
            // map merge
            result = lime_map(&frame, (u64) ((frame.registers[0]->map.capacity + frame.registers[1]->map.capacity) * (4.0 / 3.0)));
            
            if (!result.failure) {
                frame.registers[2] = result.value;

                // add the first map
                for (u64 i = 0; i < frame.registers[0]->map.capacity; ++i) {
                    temporary.registers[0] = frame.registers[0]->map.buckets[i];

                    while (temporary.registers[0] != NULL) {
                        result = lime_map_put(&frame, frame.registers[2], temporary.registers[0]->list.head, temporary.registers[0]->list.tail->list.head);

                        if (result.failure) {
                            return result.exception;
                        } else {
                            frame.registers[2] = result.value;
                        }

                        temporary.registers[0] = temporary.registers[0]->list.tail->list.tail;
                    }
                }

                // add the second map
                for (u64 i = 0; i < frame.registers[1]->map.capacity; ++i) {
                    temporary.registers[0] = frame.registers[1]->map.buckets[i];

                    while (temporary.registers[0] != NULL && !result.failure) {
                        result = lime_map_put(&frame, frame.registers[2], temporary.registers[0]->list.head, temporary.registers[0]->list.tail->list.head);
                        
                        if (result.failure) {
                            return result.exception;
                        } else {
                            frame.registers[2] = result.value;
                        }
                        
                        temporary.registers[0] = temporary.registers[0]->list.tail->list.tail;
                    }
                }

                result.value = frame.registers[2];
            }

            break;
        case LimeStringValue:
            // string concatenation
            result = lime_allocate(&frame, LimeStringValue, frame.registers[0]->string.length + frame.registers[1]->string.length);

            if (!result.failure) {
                memcpy(result.value->string.bytes, frame.registers[0]->string.bytes, frame.registers[0]->string.length);
                memcpy(result.value->string.bytes + frame.registers[0]->string.length, frame.registers[1]->string.bytes, frame.registers[1]->string.length);
                result.value->string.length = frame.registers[0]->string.length + frame.registers[1]->string.length;
            }

            break;
        case LimeNumberValue:
            result = lime_number(&frame, frame.registers[0]->number.value + frame.registers[1]->number.value);
            break;
        default:
            return lime_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'list', 'string', 'number' or 'map')", lime_value_type(frame.registers[0]), __FUNCTION__);
    }

    if (result.failure) {
        return result.exception;
    }

    result = lime_list(&frame, result.value, *frame.datastack);

    if (result.failure) {
        return result.exception;
    } else {
        *frame.datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_contains(LimeStack stack) {
    LimeValue needle;
    LimeValue haystack;

    LIME_STACK_POP_VALUE(stack, &needle);
    LIME_STACK_POP_VALUE(stack, &haystack);

    bool contains;
    if (haystack == NULL) {
        contains = false;
    } else if (haystack->type == LimeListValue) {
        contains = false;
        do {
            if (lime_value_equals(haystack->list.head, needle)) {
                contains = true;
                break;
            }

            haystack = haystack->list.tail;
        } while (haystack != NULL);
    } else if (haystack->type == LimeMapValue) {
        contains = lime_map_get_or_else(haystack, needle, lime_sentinel_value) != lime_sentinel_value;
    } else {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'list' or 'map')", lime_value_type(haystack), __FUNCTION__);
    }

    LimeResult result = lime_boolean(stack, contains);

    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);

    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_nim(LimeStack stack) {
    LimeResult result = lime_map(stack, 16);

    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);

    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_put(LimeStack stack) {
    LimeValue key;
    LimeValue value;
    LimeValue map;

    LIME_STACK_POP_VALUE(stack, &key);
    LIME_STACK_POP_VALUE(stack, &value);
    LIME_STACK_POP_VALUE(stack, &map);

    if (map == NULL || map->type != LimeMapValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'map')", lime_value_type(map), __FUNCTION__);
    }

    LimeResult result = lime_map_put(stack, map, key, value);

    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);

    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_get(LimeStack stack) {
    LimeValue key;
    LimeValue value;
    LimeValue map;

    LIME_STACK_POP_VALUE(stack, &key);
    LIME_STACK_POP_VALUE(stack, &value);
    LIME_STACK_POP_VALUE(stack, &map);

    if (map == NULL || map->type != LimeMapValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'map')", lime_value_type(map), __FUNCTION__);
    }

    LimeResult result = lime_list(stack, lime_map_get_or_else(map, key, value), *stack->datastack);

    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_del(LimeStack stack) {
    return lime_exception(stack, "'del' is not yet implemented");
}

LimeValue kernel_dup(LimeStack stack) {
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    LimeResult result = lime_list(stack, (*stack->datastack)->list.head, *stack->datastack);

    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_type(LimeStack stack) {
    // static cache for type names

    static struct lime_value symbol_type = {
        .type = LimeStringValue,
        .location = NULL,
        .hash = 0,
        .symbol = {
            .length = 6,
            .bytes = "symbol"
        }
    };

    static struct lime_value list_type = {
        .type = LimeStringValue,
        .location = NULL,
        .hash = 0,
        .symbol = {
            .length = 4,
            .bytes = "list"
        }
    };

    static struct lime_value native_type = {
        .type = LimeStringValue,
        .location = NULL,
        .hash = 0,
        .symbol = {
            .length = 8,
            .bytes = "function"
        }
    };

    static struct lime_value map_type = {
        .type = LimeStringValue,
        .location = NULL,
        .hash = 0,
        .symbol = {
            .length = 3,
            .bytes = "map"
        }
    };

    static struct lime_value boolean_type = {
        .type = LimeStringValue,
        .location = NULL,
        .hash = 0,
        .symbol = {
            .length = 7,
            .bytes = "boolean"
        }
    };

    static struct lime_value number_type = {
        .type = LimeStringValue,
        .location = NULL,
        .hash = 0,
        .symbol = {
            .length = 6,
            .bytes = "number"
        }
    };

    static struct lime_value string_type = {
        .type = LimeStringValue,
        .location = NULL,
        .hash = 0,
        .symbol = {
            .length = 6,
            .bytes = "string"
        }
    };

    static struct lime_value library_type = {
        .type = LimeStringValue,
        .location = NULL,
        .hash = 0,
        .symbol = {
            .length = 7,
            .bytes = "library"
        }
    };

    static LimeValue types[] = {
        [LimeSymbolValue]  = &symbol_type,
        [LimeListValue]    = &list_type,
        [LimeNativeValue]  = &native_type,
        [LimeMapValue]     = &map_type,
        [LimeBooleanValue] = &boolean_type,
        [LimeNumberValue]  = &number_type,
        [LimeStringValue]  = &string_type,
        [LimeLibraryValue] = &library_type
    };
    
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);
   
    const LimeValue type = value == NULL ? types[LimeListValue] : types[value->type];
    const LimeResult result = lime_list(stack, type, *stack->datastack);

    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_hash(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);

    const u64 hash = lime_value_hash(value);
    LimeResult result = lime_number(stack, (double) hash);

    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);

    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_equals(LimeStack stack) {
    LimeValue a;
    LimeValue b;

    LIME_STACK_POP_VALUE(stack, &b);
    LIME_STACK_POP_VALUE(stack, &a);

    const bool equals = lime_value_equals(a, b);
    LimeResult result = lime_boolean(stack, equals);

    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);
    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_show(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);

    char *buffer;
    u64 length;
    lime_value_show(value, &buffer, &length);

    if (buffer == NULL) {
        return lime_exception_out_of_heap;
    }

    LimeResult result = lime_string(stack, (u8*) buffer, length);
    free(buffer);

    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);
    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_throw(LimeStack stack) {
    LimeValue value;
    LIME_STACK_POP_VALUE(stack, &value);
    return value;
}

LimeValue kernel_length(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);

    const u64 length = lime_value_length(value);
    switch (value == NULL ? LimeListValue : value->type) {
        case LimeSymbolValue:
        case LimeStringValue:
        case LimeListValue:
        case LimeMapValue:
            {
                LimeResult result = lime_number(stack, (double) length);
                    
                if (result.failure) {
                    return result.exception;
                }

                result = lime_list(stack, result.value, *stack->datastack);

                if (result.failure) {
                    return result.exception;
                } else {
                    *stack->datastack = result.value;
                    return NULL;   
                }
            }
        default:
            return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'list', 'string', 'symbol' or 'map')", lime_value_type(value), __FUNCTION__);
    }
}

LimeValue kernel_nil(LimeStack stack) {
    LimeResult result = lime_list(stack, NULL, *stack->datastack);
    
    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_push(LimeStack stack) {
    LimeValue head;
    LimeValue tail;

    LIME_STACK_POP_VALUE(stack, &head);
    LIME_STACK_POP_VALUE(stack, &tail);

    if (tail != NULL && tail->type != LimeListValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'list')", lime_value_type(tail), __FUNCTION__);
    }

    LimeResult result = lime_list(stack, head, tail);
    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);
    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_pop(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);

    if (frame.registers[0] != NULL && frame.registers[0]->type != LimeListValue) {
        return lime_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'list')", lime_value_type(frame.registers[0]), __FUNCTION__);
    }

    if (frame.registers[0] == NULL) {
        return lime_exception(&frame, "illegal argument (empty list) provided in function '%s'", __FUNCTION__);
    }

    LimeResult result = lime_list(&frame, frame.registers[0]->list.tail, *frame.datastack);
    
    if (result.failure) {
        return result.exception;
    }

    result = lime_list(&frame, frame.registers[0]->list.head, result.value);

    if (result.failure) {
        return result.exception;
    } else {
        *frame.datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_library(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);

    if (value == NULL || value->type != LimeStringValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'string')", lime_value_type(value), __FUNCTION__);
    }

    char *path = lime_string_to_null_terminated(value);
    if (path == NULL) {
        return lime_exception_out_of_heap;
    }

    LimeResult result = lime_library(stack, path);
    free(path);

    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);
    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_native(LimeStack stack) {
    LimeValue symbol;
    LimeValue library;

    LIME_STACK_POP_VALUE(stack, &symbol);
    LIME_STACK_POP_VALUE(stack, &library);

    if (symbol == NULL || symbol->type != LimeStringValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'string')", lime_value_type(symbol), __FUNCTION__);
    }

    if (library == NULL || library->type != LimeLibraryValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'library')", lime_value_type(library), __FUNCTION__);
    }

    char *const symbol_name = lime_string_to_null_terminated(symbol);
    if (symbol_name == NULL) {
        return lime_exception_out_of_heap;
    }

    #if defined(OS_UNIX)
        const LimeFunction function = (LimeFunction) dlsym(library->library.handle, symbol_name);
    #elif defined(OS_WINDOWS)
        const LimeFunction function = (LimeFunction) GetProcAddress(library->library.handle, symbol_name);
    #else 
        const LimeFunction function = NULL;
    #endif

    if (function == NULL) {
        const LimeValue exception = lime_exception(stack, "failed to load native function '%s' from library in function '%s'", symbol_name, __FUNCTION__);
        free(symbol_name);
        return exception;
    }
    
    free(symbol_name);

    LimeResult result = lime_function(stack, library, function);
    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);
    if (result.failure) {
        return result.exception;
    } else {
        *stack->datastack = result.value;
        return NULL;
    }
}

LimeValue kernel_run(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LimeValue datastack;
    LimeValue callstack;
    LimeValue dictionary;

    LIME_STACK_POP_VALUE(&frame, &datastack);
    LIME_STACK_POP_VALUE(&frame, &callstack);
    LIME_STACK_POP_VALUE(&frame, &dictionary);

    frame.datastack = &datastack;
    frame.callstack = &callstack;
    frame.dictionary = &dictionary;

    if (datastack != NULL && datastack->type != LimeListValue) {
        return lime_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'list')", lime_value_type(datastack), __FUNCTION__);
    }

    if (callstack != NULL && callstack->type != LimeListValue) {
        return lime_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'list')", lime_value_type(callstack), __FUNCTION__);
    }

    if (dictionary == NULL || dictionary->type != LimeMapValue) {
        return lime_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'map')", lime_value_type(dictionary), __FUNCTION__);
    }

    LimeResult result;
    while (callstack != NULL) {
        const LimeValue instruction = callstack->list.head;
        callstack = callstack->list.tail;

        switch (instruction == NULL ? LimeListValue : instruction->type) {
            case LimeSymbolValue:
                {
                    // look up the symbol in dictionary and push associated value to the callstack
                    frame.registers[0] = lime_map_get_or_else(dictionary, instruction, lime_sentinel_value);
                    if (frame.registers[0] == lime_sentinel_value) {
                        // symbol not found => push it to the datastack
                        result = lime_list(&frame, instruction, datastack);

                        if (result.failure) {
                            return result.exception;
                        }

                        datastack = result.value;
                    } else if (frame.registers[0]->type == LimeListValue) {
                        // push the associated list to the callstack
                        while (frame.registers[0] != NULL) {
                            result = lime_list(&frame, frame.registers[0]->list.head, callstack);

                            if (result.failure) {
                                return result.exception;
                            }

                            callstack = result.value;
                            frame.registers[0] = frame.registers[0]->list.tail;
                        }
                    } else if (frame.registers[0]->type == LimeNativeValue) {
                        const LimeFunction function = frame.registers[0]->function.function;
                        const LimeValue exception = function(&frame);
                        if (exception != NULL) {
                            return exception;
                        }
                    } else {
                        result = lime_list(&frame, frame.registers[0], callstack);

                        if (result.failure) {
                            return result.exception;
                        }

                        callstack = result.value;
                    }
                }
                break;
            default:
                result = lime_list(&frame, instruction, datastack);

                if (result.failure) {
                    return result.exception;
                }

                datastack = result.value;
                break;
        }

        lime_value_dump(stdout, datastack);
        fputc('\n', stdout); 
        fflush(stdout);
    }

    return NULL;
}
