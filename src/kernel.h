#ifndef KERNEL_H
#define KERNEL_H

#include <bowl/api.h>
#include "primitives/boolean.h"
#include "primitives/list.h"
#include "primitives/map.h"
#include "primitives/number.h"
#include "primitives/string.h"
#include "primitives/symbol.h"
#include "primitives/vector.h"
#include "primitives/exception.h"

/**
 * Tokenizes the topmost value on the stack and pushes the resulting list.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_tokens(BowlStack stack);

/**
 * Swaps the two topmost elements on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_swap(BowlStack stack);

/**
 * Rotates the three topmost elements on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_rot(BowlStack stack);

/**
 * Drops the topmost element off the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_drop(BowlStack stack);

/**
 * Pushes two copies of the topmost value on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_dup(BowlStack stack);

/**
 * Pushes the type of the topmost value as a string.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_type(BowlStack stack);

/**
 * Pushes the hash of the topmost value as a number.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_hash(BowlStack stack);

/**
 * Pushes whether or not the two topmost values are equal.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_equals(BowlStack stack);

/**
 * Pushes a string representation of the topmost value.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_show(BowlStack stack);

/**
 * Triggers an exception by throwing the topmost value of the stack.
 * @param stack The stack of the current environment.
 * @return This function always returns the triggered exception.
 */
BowlValue kernel_trigger(BowlStack stack);

/**
 * Pushes a library value denoted by topmost path value on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_library(BowlStack stack);

/**
 * Pushes a native function value denoted by the two topmost values on the stack.
 * While the topmost value has to be the name of the native function, the second
 * value has to be a library value whose underlying shared library contains an equally
 * named symbol.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_native(BowlStack stack);

/**
 * Expects a datastack, callstack and dictionary as the topmost values and creates
 * a new execution environment using them.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_run(BowlStack stack);

/**
 * Lifts the current continuation on the datastack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_lift(BowlStack stack);

/**
 * Continues with the continuation which resides on the stack.
 * @param stack The stack of the current environment.
 * @return Either 'NULL' in case of a success or the exception value.
 */
BowlValue kernel_continue(BowlStack stack);

#endif
