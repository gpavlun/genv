#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Function identity.
 *
 * Any function pointer can be converted to this type
 * and used as an identity.
 */
typedef void (*function_id_t)(void);

/*
 * Error codes.
 *
 * Zero is reserved for no error.
 * UINT32_MAX is reserved for memory allocation failure.
 */
typedef uint32_t error_code_t;

#define ERR_NONE   ((error_code_t)0)
#define ERR_MEMORY UINT32_MAX

/*
 * Internal error manager functions.
 */
void error_clear(function_id_t function);

bool error_set(function_id_t function, error_code_t error);

error_code_t error_get(function_id_t function);

bool error_exists(function_id_t function);

/*
 * Public convenience interface.
 *
 * Accepts ordinary function names rather than requiring
 * explicit function pointer casts at every call site.
 */
#define clear_err(function) \
    error_clear((function_id_t)(function))

#define set_err(function, code) \
    error_set((function_id_t)(function), (code))

#define get_err(function) \
    error_get((function_id_t)(function))

#define if_err(function) \
    if (error_exists((function_id_t)(function)))

/*
 * Release error manager resources for the current thread.
 */
void error_shutdown(void);

#endif