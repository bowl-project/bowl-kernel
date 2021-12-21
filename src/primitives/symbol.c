#include "symbol.h"

BowlValue kernel_symbol_length(BowlStack stack) {
    BowlValue symbol;

    BOWL_STACK_POP_VALUE(stack, &symbol);
    BOWL_ASSERT_TYPE(symbol, BowlSymbolValue);

    BOWL_TRY(&symbol, bowl_number(stack, symbol->symbol.length));
    BOWL_STACK_PUSH_VALUE(stack, symbol);

    return NULL;
}
