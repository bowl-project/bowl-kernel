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
    { .name = "vector:list", .function = kernel_vector_list },
    { .name = "vector:reverse", .function = kernel_vector_reverse }
};

BowlValue bowl_module_initialize(BowlStack stack, BowlValue library) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, library, NULL, NULL);

    for (u64 i = 0; i < sizeof(kernel_functions) / sizeof(kernel_functions[0]); ++i) {
        KernelFunctionEntry *const entry = &kernel_functions[i];
        const BowlValue exception = bowl_register_function(&frame, entry->name, frame.registers[0], entry->function);
        if (exception != NULL) {
            return exception;
        }
    }

    return NULL;    
}

BowlValue bowl_module_finalize(BowlStack stack, BowlValue library) {
    return NULL;
}


BowlValue kernel_dup(BowlStack stack) {
    if (*stack->datastack == NULL) {
        return bowl_format_exception(stack, "stack underflow in function '%s'", __FUNCTION__).value;
    }

    BOWL_STACK_PUSH_VALUE(stack, (*stack->datastack)->list.head);
    return NULL;
}

BowlValue kernel_type(BowlStack stack) {
    BOWL_STATIC_STRING(symbol_type, "symbol");
    BOWL_STATIC_STRING(list_type, "list");
    BOWL_STATIC_STRING(function_type, "function");
    BOWL_STATIC_STRING(map_type, "map");
    BOWL_STATIC_STRING(boolean_type, "boolean");
    BOWL_STATIC_STRING(number_type, "number");
    BOWL_STATIC_STRING(string_type, "string");
    BOWL_STATIC_STRING(library_type, "library");
    BOWL_STATIC_STRING(vector_type, "vector");
    BOWL_STATIC_STRING(exception_type, "exception");
  
    static BowlValue types[] = {
        [BowlSymbolValue]    = &symbol_type.value,
        [BowlListValue]      = &list_type.value,
        [BowlNativeValue]    = &function_type.value,
        [BowlMapValue]       = &map_type.value,
        [BowlBooleanValue]   = &boolean_type.value,
        [BowlNumberValue]    = &number_type.value,
        [BowlStringValue]    = &string_type.value,
        [BowlLibraryValue]   = &library_type.value,
        [BowlVectorValue]    = &vector_type.value,
        [BowlExceptionValue] = &exception_type.value
    };
    
    BowlValue value;
    BOWL_STACK_POP_VALUE(stack, &value);
   
    const BowlValue type = value == NULL ? types[BowlListValue] : types[value->type];
    BOWL_STACK_PUSH_VALUE(stack, type);

    return NULL;
}

BowlValue kernel_hash(BowlStack stack) {
    BowlValue value;

    BOWL_STACK_POP_VALUE(stack, &value);

    const u64 hash = bowl_value_hash(value);
    BOWL_TRY(&value, bowl_number(stack, (double) hash));

    BOWL_STACK_PUSH_VALUE(stack, value);
    return NULL;
}

BowlValue kernel_equals(BowlStack stack) {
    BowlValue a;
    BowlValue b;

    BOWL_STACK_POP_VALUE(stack, &b);
    BOWL_STACK_POP_VALUE(stack, &a);

    const bool equals = bowl_value_equals(a, b);
    BOWL_TRY(&a, bowl_boolean(stack, equals));

    BOWL_STACK_PUSH_VALUE(stack, a);
    return NULL;
}

BowlValue kernel_show(BowlStack stack) {
    BowlValue value;

    BOWL_STACK_POP_VALUE(stack, &value);

    char *buffer;
    u64 length;
    bowl_value_show(value, &buffer, &length);

    if (buffer == NULL) {
        return bowl_exception_out_of_heap;
    }

    BowlResult result = bowl_string(stack, (u8*) buffer, length);
    free(buffer);

    if (result.failure) {
        return result.exception;
    }

    BOWL_STACK_PUSH_VALUE(stack, result.value);
    return NULL;
}

BowlValue kernel_trigger(BowlStack stack) {
    BowlValue value;
    
    BOWL_STACK_POP_VALUE(stack, &value);
    BOWL_ASSERT_TYPE(value, BowlExceptionValue);

    return value;
}

BowlValue kernel_library(BowlStack stack) {
    BowlValue value;

    BOWL_STACK_POP_VALUE(stack, &value);

    if (value == NULL || value->type != BowlStringValue) {
        return bowl_format_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'string')", bowl_value_type(value), __FUNCTION__).value;
    }

    char *path = bowl_string_to_null_terminated(value);
    if (path == NULL) {
        return bowl_exception_out_of_heap;
    }

    BowlResult result = bowl_library(stack, path);
    free(path);

    if (result.failure) {
        return result.exception;
    }

    BOWL_STACK_PUSH_VALUE(stack, result.value);
    return NULL;
}

BowlValue kernel_native(BowlStack stack) {
    BowlValue symbol;
    BowlValue library;

    BOWL_STACK_POP_VALUE(stack, &symbol);
    BOWL_STACK_POP_VALUE(stack, &library);

    if (symbol == NULL || symbol->type != BowlStringValue) {
        return bowl_format_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'string')", bowl_value_type(symbol), __FUNCTION__).value;
    }

    if (library == NULL || library->type != BowlLibraryValue) {
        return bowl_format_exception(stack, "argument of illegal type '%s' provided in function '%s' (expected 'library')", bowl_value_type(library), __FUNCTION__).value;
    }

    char *const symbol_name = bowl_string_to_null_terminated(symbol);
    if (symbol_name == NULL) {
        return bowl_exception_out_of_heap;
    }

    #if defined(OS_UNIX)
        const BowlFunction function = (BowlFunction) dlsym(library->library.handle, symbol_name);
    #elif defined(OS_WINDOWS)
        const BowlFunction function = (BowlFunction) GetProcAddress(library->library.handle, symbol_name);
    #else 
        const BowlFunction function = NULL;
    #endif

    if (function == NULL) {
        const BowlValue exception = bowl_format_exception(stack, "failed to load native function '%s' from library in function '%s'", symbol_name, __FUNCTION__).value;
        free(symbol_name);
        return exception;
    }
    
    free(symbol_name);

    BOWL_TRY(&symbol, bowl_function(stack, library, function));
    BOWL_STACK_PUSH_VALUE(stack, symbol);

    return NULL;
}

BowlValue kernel_run(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BowlValue datastack;
    BowlValue callstack;
    BowlValue dictionary;

    BOWL_STACK_POP_VALUE(&frame, &datastack);
    BOWL_STACK_POP_VALUE(&frame, &callstack);
    BOWL_STACK_POP_VALUE(&frame, &dictionary);

    frame.datastack = &datastack;
    frame.callstack = &callstack;
    frame.dictionary = &dictionary;

    if (datastack != NULL && datastack->type != BowlListValue) {
        return bowl_format_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'list')", bowl_value_type(datastack), __FUNCTION__).value;
    }

    if (callstack != NULL && callstack->type != BowlListValue) {
        return bowl_format_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'list')", bowl_value_type(callstack), __FUNCTION__).value;
    }

    if (dictionary == NULL || dictionary->type != BowlMapValue) {
        return bowl_format_exception(&frame, "argument of illegal type '%s' provided in function '%s' (expected 'map')", bowl_value_type(dictionary), __FUNCTION__).value;
    }

    BowlResult result;

    BowlValue exception = NULL;
    while (callstack != NULL) {
        const BowlValue instruction = callstack->list.head;
        callstack = callstack->list.tail;

        if (bowl_settings_verbosity > 0) {
            fputc('\n', stdout); 
        }

        if (bowl_settings_verbosity > 1) {
            printf("[instruction] ");
            bowl_value_dump(stdout, instruction);
            fputc('\n', stdout);
            fflush(stdout);
        }

        switch (instruction == NULL ? BowlListValue : instruction->type) {
            case BowlSymbolValue:
                {
                    // look up the symbol in dictionary and push associated value to the callstack
                    frame.registers[0] = bowl_map_get_or_else(dictionary, instruction, bowl_sentinel_value);
                    if (frame.registers[0] == bowl_sentinel_value) {
                        // symbol not found => push it to the datastack
                        result = bowl_list(&frame, instruction, datastack);

                        if (result.failure) {
                            exception = result.exception;
                            goto exception_break_point;
                        }

                        datastack = result.value;
                    } else if (frame.registers[0]->type == BowlListValue) {
                        // push the associated list to the callstack
                        while (frame.registers[0] != NULL) {
                            result = bowl_list(&frame, frame.registers[0]->list.head, callstack);

                            if (result.failure) {
                                exception = result.exception;
                                goto exception_break_point;
                            }

                            callstack = result.value;
                            frame.registers[0] = frame.registers[0]->list.tail;
                        }
                    } else if (frame.registers[0]->type == BowlNativeValue) {
                        const BowlFunction function = frame.registers[0]->function.function;
                        exception = function(&frame);
                        if (exception != NULL) {
                            goto exception_break_point;    
                        }
                    } else {
                        result = bowl_list(&frame, frame.registers[0], callstack);

                        if (result.failure) {
                            exception = result.exception;
                            goto exception_break_point;
                        }

                        callstack = result.value;
                    }
                }
                break;
            default:
                result = bowl_list(&frame, instruction, datastack);

                if (result.failure) {
                    exception = result.exception;
                    goto exception_break_point;
                }

                datastack = result.value;
                break;
        }

        if (bowl_settings_verbosity > 0) {
            bowl_value_dump(stdout, datastack);
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

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[2]);
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]);

    return NULL;
}

BowlValue kernel_lift(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, *stack->datastack, NULL, NULL);
    
    *frame.datastack = NULL;
    BOWL_STACK_PUSH_VALUE(&frame, *frame.dictionary);
    BOWL_STACK_PUSH_VALUE(&frame, *frame.callstack);
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]);
    
    return NULL;
}

BowlValue kernel_continue(BowlStack stack) {
    BowlValue datastack;
    BowlValue callstack;
    BowlValue dictionary;

    BOWL_STACK_POP_VALUE(stack, &datastack);
    BOWL_ASSERT_TYPE(datastack, BowlListValue);

    BOWL_STACK_POP_VALUE(stack, &callstack);
    BOWL_ASSERT_TYPE(callstack, BowlListValue);

    BOWL_STACK_POP_VALUE(stack, &dictionary);
    BOWL_ASSERT_TYPE(dictionary, BowlMapValue);

    *stack->datastack = datastack;
    *stack->callstack = callstack;
    *stack->dictionary = dictionary;
    
    return NULL;
}

BowlValue kernel_tokens(BowlStack stack) {
    BowlValue string;

    BOWL_STACK_POP_VALUE(stack, &string);
    BOWL_TRY(&string, bowl_tokens(stack, string));
    BOWL_STACK_PUSH_VALUE(stack, string);
    
    return NULL;
}

BowlValue kernel_swap(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_STACK_POP_VALUE(&frame, &frame.registers[1]);
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]);
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

BowlValue kernel_rot(BowlStack stack) {
    BowlStackFrame frame = BOWL_ALLOCATE_STACK_FRAME(stack, NULL, NULL, NULL);

    BOWL_STACK_POP_VALUE(&frame, &frame.registers[0]);
    BOWL_STACK_POP_VALUE(&frame, &frame.registers[1]);
    BOWL_STACK_POP_VALUE(&frame, &frame.registers[2]);

    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[0]);
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[2]);
    BOWL_STACK_PUSH_VALUE(&frame, frame.registers[1]);

    return NULL;
}

BowlValue kernel_drop(BowlStack stack) {
    BowlValue value;
    BOWL_STACK_POP_VALUE(stack, &value);
    return NULL;
}
