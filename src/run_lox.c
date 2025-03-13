#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "vm.h"
#include "compiler.h"

/* 
 * If compiling under a C++ compiler or for Emscripten,
 * you can wrap with extern "C" to avoid name-mangling.
 */
#ifdef __cplusplus
extern "C" {
#endif

/* 
 * run_lox: A function you can export to WebAssembly.
 * 
 * Returns:
 *   0 -> INTERPRET_OK
 *   1 -> INTERPRET_COMPILE_ERROR
 *   2 -> INTERPRET_RUNTIME_ERROR
 */
int run_lox(const char* source) {
    initVM();
    
    InterpretResult result = interpret(source);

    freeVM();

    switch (result) {
        case INTERPRET_OK:
            return 0;
        case INTERPRET_COMPILE_ERROR:
            return 1;
        case INTERPRET_RUNTIME_ERROR:
            return 2;
    }
    // Should never get here, but just in case:
    return 3;
}

#ifdef __cplusplus
}
#endif
