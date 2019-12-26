#include "kernel.h"

static KernelFunctionEntry kernel_functions[] = {
    { .name = "run", .function = kernel_run },
    { .name = "dup", .function = kernel_dup },
    { .name = "type", .function = kernel_type },
    { .name = "hash", .function = kernel_hash },
    { .name = "equals", .function = kernel_equals },
    { .name = "show", .function = kernel_show },
    { .name = "throw", .function = kernel_throw },
    { .name = "lift", .function = kernel_lift },
    { .name = "continue", .function = kernel_continue },
    { .name = "library", .function = kernel_library },
    { .name = "native", .function = kernel_native },

    { .name = "list:length", .function = kernel_list_length },
    { .name = "list:empty", .function = kernel_list_empty },
    { .name = "list:push", .function = kernel_list_push },
    { .name = "list:pop", .function = kernel_list_pop },
    { .name = "list:concat", .function = kernel_list_concat },
    { .name = "list:contains", .function = kernel_list_contains },

    { .name = "map:length", .function = kernel_map_length },
    { .name = "map:empty", .function = kernel_map_empty },
    { .name = "map:put", .function = kernel_map_put },
    { .name = "map:get", .function = kernel_map_get },
    { .name = "map:del", .function = kernel_map_del },
    { .name = "map:merge", .function = kernel_map_merge },
    { .name = "map:contains", .function = kernel_map_contains },

    { .name = "number:add", .function = kernel_number_add },
    { .name = "number:subtract", .function = kernel_number_subtract },
    { .name = "number:multiply", .function = kernel_number_multiply },
    { .name = "number:divide", .function = kernel_number_divide },
    { .name = "number:remainder", .function = kernel_number_remainder },

    { .name = "boolean:and", .function = kernel_boolean_and },
    { .name = "boolean:or", .function = kernel_boolean_or },
    { .name = "boolean:xor", .function = kernel_boolean_xor },
    { .name = "boolean:not", .function = kernel_boolean_not },

    { .name = "string:length", .function = kernel_string_length },
    { .name = "string:concat", .function = kernel_string_concat },
    { .name = "string:slice", .function = kernel_string_slice },
    { .name = "string:number", .function = kernel_string_number },
    { .name = "string:boolean", .function = kernel_string_boolean },
    { .name = "string:symbol", .function = kernel_string_symbol },

    { .name = "symbol:length", .function = kernel_symbol_length }
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


LimeValue kernel_dup(LimeStack stack) {
    if (*stack->datastack == NULL) {
        return lime_exception(stack, "stack underflow in function '%s'", __FUNCTION__);
    }

    LIME_STACK_PUSH_VALUE(stack, (*stack->datastack)->list.head);
    return NULL;
}

LimeValue kernel_type(LimeStack stack) {
    // static cache for type names
    static struct lime_value symbol_type = LIME_STRING_STATIC_CONSTANT("symbol");
    static struct lime_value list_type = LIME_STRING_STATIC_CONSTANT("list");
    static struct lime_value native_type = LIME_STRING_STATIC_CONSTANT("function");
    static struct lime_value map_type = LIME_STRING_STATIC_CONSTANT("map");
    static struct lime_value boolean_type = LIME_STRING_STATIC_CONSTANT("boolean");
    static struct lime_value number_type = LIME_STRING_STATIC_CONSTANT("number");
    static struct lime_value string_type = LIME_STRING_STATIC_CONSTANT("string");
    static struct lime_value library_type = LIME_STRING_STATIC_CONSTANT("library");
  
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
    LIME_STACK_PUSH_VALUE(stack, type);

    return NULL;
}

LimeValue kernel_hash(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);

    const u64 hash = lime_value_hash(value);
    LIME_TRY(&value, lime_number(stack, (double) hash));

    LIME_STACK_PUSH_VALUE(stack, value);
    return NULL;
}

LimeValue kernel_equals(LimeStack stack) {
    LimeValue a;
    LimeValue b;

    LIME_STACK_POP_VALUE(stack, &b);
    LIME_STACK_POP_VALUE(stack, &a);

    const bool equals = lime_value_equals(a, b);
    LIME_TRY(&a, lime_boolean(stack, equals));

    LIME_STACK_PUSH_VALUE(stack, a);
    return NULL;
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

    LIME_STACK_PUSH_VALUE(stack, result.value);
    return NULL;
}

LimeValue kernel_throw(LimeStack stack) {
    LimeValue value;
    LIME_STACK_POP_VALUE(stack, &value);
    return value;
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

    LIME_STACK_PUSH_VALUE(stack, result.value);
    return NULL;
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

    LIME_TRY(&symbol, lime_function(stack, library, function));
    LIME_STACK_PUSH_VALUE(stack, symbol);

    return NULL;
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

        if (lime_settings_verbosity > 0) {
            lime_value_dump(stdout, datastack);
            fputc('\n', stdout); 
            fflush(stdout);
        }
    }

    return NULL;
}

LimeValue kernel_lift(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, *stack->datastack, NULL, NULL);
    
    *frame.datastack = NULL;
    LIME_STACK_PUSH_VALUE(&frame, *frame.dictionary);
    LIME_STACK_PUSH_VALUE(&frame, *frame.callstack);
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[0]);
    
    return NULL;
}

LimeValue kernel_continue(LimeStack stack) {
    LimeValue datastack;
    LimeValue callstack;
    LimeValue dictionary;

    LIME_STACK_POP_VALUE(stack, &datastack);
    LIME_ASSERT_TYPE(datastack, LimeListValue);

    LIME_STACK_POP_VALUE(stack, &callstack);
    LIME_ASSERT_TYPE(datastack, LimeListValue);

    LIME_STACK_POP_VALUE(stack, &dictionary);
    LIME_ASSERT_TYPE(datastack, LimeMapValue);

    *stack->datastack = datastack;
    *stack->callstack = callstack;
    *stack->dictionary = dictionary;
    
    return NULL;
}
