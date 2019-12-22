#ifndef KERNEL_H
#define KERNEL_H

#include <lime/api.h>

typedef struct {
    char *name;
    LimeFunction function;
} KernelFunctionEntry;

/**
 * Adds the two topmost values and pushes the result on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_add(LimeStack stack);

/**
 * Checks whether the topmost value is an element of the topmost but one value.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_contains(LimeStack stack);

/**
 * Pushes the empty map value on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_nim(LimeStack stack);

/**
 * Expects a key, a value and a map as the three topmost values on the stack. 
 * Inserts the value into the map using the provided key. If there is already a
 * value associated with this key, the old value will be overwritten with the new
 * one.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_put(LimeStack stack);

/**
 * Expects a key, a value and a map as the three topmost values on the stack.
 * Fetches the value which is associated with the provided key from the map. If
 * there is no value associated with this key, the provided one is pushed on
 * the stack instead.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_get(LimeStack stack);

/**
 * Expects a key and a map as the two topmost values on the stack.
 * Deletes the value which is associated with the provided key from the map and
 * pushes the resulting map on the stack. If there was no value associated with 
 * this key, the the provided map is pushed unchanged.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_del(LimeStack stack);

/**
 * Pushes two copies of the topmost value on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_dup(LimeStack stack);

/**
 * Pushes the type of the topmost value as a string.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_type(LimeStack stack);

/**
 * Pushes the hash of the topmost value as a number.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_hash(LimeStack stack);

/**
 * Pushes whether or not the two topmost values are equal.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_equals(LimeStack stack);

/**
 * Pushes a string representation of the topmost value.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_show(LimeStack stack);

/**
 * Throws the topmost value as an exception.
 * @param stack The stack of the current environment.
 * @return This function always returns the thrown exception.
 */
LimeValue kernel_throw(LimeStack stack);

/**
 * Pushes the length of the topmost value as a number.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_length(LimeStack stack);

/**
 * Pushes the empty list on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_nil(LimeStack stack);

/**
 * Pushes a new list consisting of the two topmost values on the stack. 
 * While the topmost value has to be a list, the second one may be an arbitrary
 * head element.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_push(LimeStack stack);

/**
 * Pushes the head and tail of the topmost list value on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_pop(LimeStack stack);

/**
 * Pushes a library value denoted by topmost path value on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_library(LimeStack stack);

/**
 * Pushes a native function value denoted by the two topmost values on the stack.
 * While the topmost value has to be the name of the native function, the second
 * value has to be a library value whose underlying shared library contains an equally
 * named symbol.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_native(LimeStack stack);

/**
 * Expects a datastack, callstack and dictionary as the topmost values and creates
 * a new execution environment using them.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_run(LimeStack stack);

#endif
