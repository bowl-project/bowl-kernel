#ifndef KERNEL_H
#define KERNEL_H

#include <lime/api.h>
#include "primitives/boolean.h"
#include "primitives/list.h"
#include "primitives/map.h"
#include "primitives/number.h"
#include "primitives/string.h"
#include "primitives/symbol.h"

typedef struct {
    char *name;
    LimeFunction function;
} KernelFunctionEntry;

/**
 * Tokenizes the topmost value on the stack and pushes the resulting list.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_tokens(LimeStack stack);

/**
 * Swaps the two topmost elements on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_swap(LimeStack stack);

/**
 * Rotates the three topmost elements on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_rot(LimeStack stack);

/**
 * Drops the topmost element off the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_drop(LimeStack stack);

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

/**
 * Lifts the current continuation on the datastack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_lift(LimeStack stack);

/**
 * Continues with the continuation which resides on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
LimeValue kernel_continue(LimeStack stack);

#endif
