#include "kernel.h"

static KernelFunctionEntry kernel_functions[] = {
    { .name = "run", .function = kernel_run },
    { .name = "dup", .function = kernel_dup },
    { .name = "type", .function = kernel_type },
    { .name = "hash", .function = kernel_hash },
    { .name = "equals", .function = kernel_equals },
    { .name = "show", .function = kernel_show },
    { .name = "trigger", .function = kernel_trigger },
    { .name = "exception", .function = kernel_exception },
    { .name = "lift", .function = kernel_lift },
    { .name = "continue", .function = kernel_continue },
    { .name = "library", .function = kernel_library },
    { .name = "native", .function = kernel_native },
    { .name = "tokens", .function = kernel_tokens },
    { .name = "swap", .function = kernel_swap },
    { .name = "rot", .function = kernel_rot },
    { .name = "drop", .function = kernel_drop },

    { .name = "list:length", .function = kernel_list_length },
    { .name = "list:empty", .function = kernel_list_empty },
    { .name = "list:push", .function = kernel_list_push },
    { .name = "list:pop", .function = kernel_list_pop },
    { .name = "list:concat", .function = kernel_list_concat },
    { .name = "list:contains", .function = kernel_list_contains },
    { .name = "list:slice", .function = kernel_list_slice },
    { .name = "list:reverse", .function = kernel_list_reverse },
    { .name = "list:vector", .function = kernel_list_vector },

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
    { .name = "number:less-than", .function = kernel_number_less_than },
    { .name = "number:less-equal", .function = kernel_number_less_equal },
    { .name = "number:greater-than", .function = kernel_number_greater_than },
    { .name = "number:greater-equal", .function = kernel_number_greater_equal },
    
    { .name = "boolean:and", .function = kernel_boolean_and },
    { .name = "boolean:or", .function = kernel_boolean_or },
    { .name = "boolean:xor", .function = kernel_boolean_xor },
    { .name = "boolean:not", .function = kernel_boolean_not },
    { .name = "boolean:choose", .function = kernel_boolean_choose },

    { .name = "string:length", .function = kernel_string_length },
    { .name = "string:concat", .function = kernel_string_concat },
    { .name = "string:slice", .function = kernel_string_slice },
    { .name = "string:number", .function = kernel_string_number },
    { .name = "string:boolean", .function = kernel_string_boolean },
    { .name = "string:symbol", .function = kernel_string_symbol },

    { .name = "symbol:length", .function = kernel_symbol_length },

    { .name = "vector:empty", .function = kernel_vector_empty },
    { .name = "vector:fill", .function = kernel_vector_fill },
    { .name = "vector:length", .function = kernel_vector_length },
    { .name = "vector:concat", .function = kernel_vector_concat },
    { .name = "vector:slice", .function = kernel_vector_slice },
    { .name = "vector:list", .function = kernel_vector_list }
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
        return lime_format_exception(stack, "stack underflow in function '%s'", __FUNCTION__).value;
    }

    LIME_STACK_PUSH_VALUE(stack, (*stack->datastack)->list.head);
    return NULL;
}

LimeValue kernel_type(LimeStack stack) {
    LIME_STATIC_STRING(symbol_type, "symbol");
    LIME_STATIC_STRING(list_type, "list");
    LIME_STATIC_STRING(function_type, "function");
    LIME_STATIC_STRING(map_type, "map");
    LIME_STATIC_STRING(boolean_type, "boolean");
    LIME_STATIC_STRING(number_type, "number");
    LIME_STATIC_STRING(string_type, "string");
    LIME_STATIC_STRING(library_type, "library");
    LIME_STATIC_STRING(vector_type, "vector");
    LIME_STATIC_STRING(exception_type, "exception");
  
    static LimeValue types[] = {
        [LimeSymbolValue]    = &symbol_type.value,
        [LimeListValue]      = &list_type.value,
        [LimeNativeValue]    = &function_type.value,
        [LimeMapValue]       = &map_type.value,
        [LimeBooleanValue]   = &boolean_type.value,
        [LimeNumberValue]    = &number_type.value,
        [LimeStringValue]    = &string_type.value,
        [LimeLibraryValue]   = &library_type.value,
        [LimeVectorValue]    = &vector_type.value,
        [LimeExceptionValue] = &exception_type.value
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

LimeValue kernel_trigger(LimeStack stack) {
    LimeValue value;
    
    LIME_STACK_POP_VALUE(stack, &value);
    LIME_ASSERT_TYPE(value, LimeExceptionValue);

    return value;
}

LimeValue kernel_library(LimeStack stack) {
    LimeValue value;

    LIME_STACK_POP_VALUE(stack, &value);

    if (value == NULL || value->type != LimeStringValue) {
        return lime_format_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'string')", lime_value_type(value), __FUNCTION__).value;
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
        return lime_format_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'string')", lime_value_type(symbol), __FUNCTION__).value;
    }

    if (library == NULL || library->type != LimeLibraryValue) {
        return lime_format_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'library')", lime_value_type(library), __FUNCTION__).value;
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
        const LimeValue exception = lime_format_exception(stack, "failed to load native function '%s' from library in function '%s'", symbol_name, __FUNCTION__).value;
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
        return lime_format_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'list')", lime_value_type(datastack), __FUNCTION__).value;
    }

    if (callstack != NULL && callstack->type != LimeListValue) {
        return lime_format_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'list')", lime_value_type(callstack), __FUNCTION__).value;
    }

    if (dictionary == NULL || dictionary->type != LimeMapValue) {
        return lime_format_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'map')", lime_value_type(dictionary), __FUNCTION__).value;
    }

    LimeResult result;

    LimeValue exception = NULL;
    while (callstack != NULL) {
        const LimeValue instruction = callstack->list.head;
        callstack = callstack->list.tail;

        if (lime_settings_verbosity > 0) {
            fputc('\n', stdout); 
        }

        if (lime_settings_verbosity > 1) {
            printf("[instruction] ");
            lime_value_dump(stdout, instruction);
            fputc('\n', stdout);
            fflush(stdout);
        }

        switch (instruction == NULL ? LimeListValue : instruction->type) {
            case LimeSymbolValue:
                {
                    // look up the symbol in dictionary and push associated value to the callstack
                    frame.registers[0] = lime_map_get_or_else(dictionary, instruction, lime_sentinel_value);
                    if (frame.registers[0] == lime_sentinel_value) {
                        // symbol not found => push it to the datastack
                        result = lime_list(&frame, instruction, datastack);

                        if (result.failure) {
                            exception = result.exception;
                            goto exception_break_point;
                        }

                        datastack = result.value;
                    } else if (frame.registers[0]->type == LimeListValue) {
                        // push the associated list to the callstack
                        while (frame.registers[0] != NULL) {
                            result = lime_list(&frame, frame.registers[0]->list.head, callstack);

                            if (result.failure) {
                                exception = result.exception;
                                goto exception_break_point;
                            }

                            callstack = result.value;
                            frame.registers[0] = frame.registers[0]->list.tail;
                        }
                    } else if (frame.registers[0]->type == LimeNativeValue) {
                        const LimeFunction function = frame.registers[0]->function.function;
                        exception = function(&frame);
                        if (exception != NULL) {
                            goto exception_break_point;    
                        }
                    } else {
                        result = lime_list(&frame, frame.registers[0], callstack);

                        if (result.failure) {
                            exception = result.exception;
                            goto exception_break_point;
                        }

                        callstack = result.value;
                    }
                }
                break;
            default:
                result = lime_list(&frame, instruction, datastack);

                if (result.failure) {
                    exception = result.exception;
                    goto exception_break_point;
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

    exception_break_point:
    // restore the stacks and the dictionary of the previous instance
    frame.datastack = stack->datastack;
    frame.callstack = stack->callstack;
    frame.dictionary = stack->dictionary;

    // save the stacks and the dictionary of this run
    frame.registers[0] = datastack;
    frame.registers[1] = exception;
    frame.registers[2] = dictionary;

    LIME_STACK_PUSH_VALUE(&frame, frame.registers[2]);
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[1]);
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[0]);

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
    LIME_ASSERT_TYPE(callstack, LimeListValue);

    LIME_STACK_POP_VALUE(stack, &dictionary);
    LIME_ASSERT_TYPE(dictionary, LimeMapValue);

    *stack->datastack = datastack;
    *stack->callstack = callstack;
    *stack->dictionary = dictionary;
    
    return NULL;
}

LimeValue kernel_tokens(LimeStack stack) {
    LimeValue string;

    LIME_STACK_POP_VALUE(stack, &string);
    LIME_TRY(&string, lime_tokens(stack, string));
    LIME_STACK_PUSH_VALUE(stack, string);
    
    return NULL;
}

LimeValue kernel_swap(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_STACK_POP_VALUE(&frame, &frame.registers[1]);
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[0]);
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

LimeValue kernel_rot(LimeStack stack) {
    LimeStackFrame frame = LIME_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    LIME_STACK_POP_VALUE(&frame, &frame.registers[0]);
    LIME_STACK_POP_VALUE(&frame, &frame.registers[1]);
    LIME_STACK_POP_VALUE(&frame, &frame.registers[2]);

    LIME_STACK_PUSH_VALUE(&frame, frame.registers[0]);
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[2]);
    LIME_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

LimeValue kernel_drop(LimeStack stack) {
    LimeValue value;
    LIME_STACK_POP_VALUE(stack, &value);
    return NULL;
}
