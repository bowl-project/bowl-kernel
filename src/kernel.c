#include "kernel.h"

static KernelFunctionEntry kernel_functions[] = {
    { .name = "run", .function = kernel_run },
    { .name = "type", .function = kernel_type },
    { .name = "hash", .function = kernel_hash },
    { .name = "equals", .function = kernel_equals },
    { .name = "show", .function = kernel_show },
    { .name = "throw", .function = kernel_throw },
    { .name = "length", .function = kernel_length },
    { .name = "nil", .function = kernel_nil },
    { .name = "push", .function = kernel_push },
    { .name = "library", .function = kernel_library },
    { .name = "native", .function = kernel_native },
    { .name = "library-is-loaded", .function = kernel_library_is_loaded }
};

LimeValue lime_module_initialize(LimeStack stack, LimeValue library) {
    printf("[debug][kernel] initialize ...\n"); fflush(stdout);

    for (u64 i = 0; i < sizeof(kernel_functions) / sizeof(kernel_functions[0]); ++i) {
        KernelFunctionEntry *const entry = &kernel_functions[i];
        const LimeValue exception = lime_register_function(stack, entry->name, library, entry->function);
        if (exception != NULL) {
            return exception;
        }
    }

    printf("[debug][kernel] initialized!\n"); fflush(stdout);

    return NULL;    
}

LimeValue lime_module_finalize(LimeStack stack, LimeValue library) {
    printf("[debug][kernel] finalize ...\n"); fflush(stdout);
    printf("[debug][kernel] finalized!\n"); fflush(stdout);
    return NULL;
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
    
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue value = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

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
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue value = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

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
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue a = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue b = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

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
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue value = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

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
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue value = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

    return value;
}

LimeValue kernel_length(LimeStack stack) {
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue value = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

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
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue head = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue tail = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

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

LimeValue kernel_library(LimeStack stack) {
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue value = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

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
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue symbol = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

    if (symbol == NULL || symbol->type != LimeLibraryValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'string')", lime_value_type(symbol), __FUNCTION__);
    }

    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue library = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

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

    if (*stack->datastack == NULL) {
        return lime_exception(&frame, "stack underflow in function '%s'", __FUNCTION__);
    }

    LimeValue datastack = (*stack->datastack)->list.head;
    frame.datastack = &datastack;
    *stack->datastack = (*stack->datastack)->list.tail;

    if (datastack != NULL && datastack->type != LimeListValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'list')", lime_value_type(datastack), __FUNCTION__);
    } else if (*stack->datastack == NULL) {
        return lime_exception(&frame, "stack underflow in function '%s'", __FUNCTION__);
    }

    LimeValue callstack  = (*stack->datastack)->list.head;
    frame.callstack = &callstack;
    *stack->datastack = (*stack->datastack)->list.tail;

    if (callstack != NULL && callstack->type != LimeListValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'list')", lime_value_type(callstack), __FUNCTION__);
    } else if (*stack->datastack == NULL) {
        return lime_exception(&frame, "stack underflow in function '%s'", __FUNCTION__);
    }

    LimeValue dictionary = (*stack->datastack)->list.head;
    frame.dictionary = &dictionary;
    *stack->datastack = (*stack->datastack)->list.tail;

    if (dictionary == NULL || dictionary->type != LimeMapValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'map')", lime_value_type(dictionary), __FUNCTION__);
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

LimeValue kernel_library_is_loaded(LimeStack stack) {
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    const LimeValue value = (*stack->datastack)->list.head;
    *stack->datastack = (*stack->datastack)->list.tail;

    if (value == NULL || value->type != LimeStringValue) {
        return lime_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'string')", lime_value_type(value), __FUNCTION__);
    }

    char *path = lime_string_to_null_terminated(value);
    if (path == NULL) {
        return lime_exception_out_of_heap;
    }

    bool loaded = lime_library_is_loaded(path);
    free(path);

    LimeResult result = lime_boolean(stack, loaded);
    
    if (result.failure) {
        return result.exception;
    }

    result = lime_list(stack, result.value, *stack->datastack);
    
    if (result.failure) {
        return result.exception;
    }

    *stack->datastack = result.value;

    return NULL;    
}
