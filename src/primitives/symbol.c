#include "symbol.h"

LimeValue kernel_symbol_length(LimeStack stack) {
    LimeValue symbol;

    LIME_STACK_POP_VALUE(stack, &symbol);
    LIME_ASSERT_TYPE(symbol, LimeSymbolValue);

    LIME_TRY(&symbol, lime_number(stack, symbol->symbol.length));
    LIME_STACK_PUSH_VALUE(stack, symbol);

    return NULL;
}
