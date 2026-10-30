#include "error.h"

#include <stdlib.h>

/*
 * Regular error storage.
 */
typedef struct error_entry {
    function_id_t function;
    error_code_t error;

    struct error_entry *next;
} error_entry_t;

/*
 * Emergency memory failure storage.
 *
 * This structure never requires dynamic allocation.
 * Only the most recent allocation failure is preserved.
 */
typedef struct {
    bool active;
    function_id_t function;
} emergency_error_t;

/*
 * Thread-local error manager state.
 */
static _Thread_local error_entry_t *error_entries = NULL;

static _Thread_local emergency_error_t emergency_error = {
    .active = false,
    .function = NULL
};

/*
 * Find an existing error entry.
 */
static error_entry_t *find_entry(function_id_t function)
{
    error_entry_t *entry = error_entries;

    while (entry != NULL) {
        if (entry->function == function)
            return entry;

        entry = entry->next;
    }

    return NULL;
}

/*
 * Obtain an existing entry or allocate a new one.
 */
static error_entry_t *get_entry(function_id_t function)
{
    error_entry_t *entry = find_entry(function);

    if (entry != NULL)
        return entry;

    entry = malloc(sizeof(*entry));

    if (entry == NULL)
        return NULL;

    entry->function = function;
    entry->error = ERR_NONE;

    entry->next = error_entries;
    error_entries = entry;

    return entry;
}

/*
 * Record an emergency memory failure.
 *
 * This operation never allocates memory.
 */
static void set_memory_failure(function_id_t function)
{
    emergency_error.active = true;
    emergency_error.function = function;
}

/*
 * Clear a function's error state.
 */
void error_clear(function_id_t function)
{
    if (function == NULL)
        return;

    /*
     * Clear emergency state if it belongs to this function.
     */
    if (emergency_error.active &&
        emergency_error.function == function) {

        emergency_error.active = false;
        emergency_error.function = NULL;
    }

    /*
     * Clear regular error state.
     */
    error_entry_t *entry = find_entry(function);

    if (entry != NULL)
        entry->error = ERR_NONE;
}

/*
 * Set a function's error state.
 */
bool error_set(function_id_t function, error_code_t error)
{
    if (function == NULL)
        return false;

    /*
     * Reserved memory failure code.
     */
    if (error == ERR_MEMORY) {
        set_memory_failure(function);
        return true;
    }

    /*
     * ERR_NONE is not a valid error to record.
     * Use clear_err() instead.
     */
    if (error == ERR_NONE)
        return false;

    /*
     * Obtain or create regular error storage.
     */
    error_entry_t *entry = get_entry(function);

    /*
     * Allocation failure.
     *
     * Fall back to the emergency channel.
     */
    if (entry == NULL) {
        set_memory_failure(function);
        return false;
    }

    entry->error = error;

    return true;
}

/*
 * Retrieve a function's current error.
 */
error_code_t error_get(function_id_t function)
{
    if (function == NULL)
        return ERR_NONE;

    /*
     * Emergency state takes priority for its function.
     */
    if (emergency_error.active &&
        emergency_error.function == function) {

        return ERR_MEMORY;
    }

    /*
     * Check regular storage.
     */
    error_entry_t *entry = find_entry(function);

    if (entry == NULL)
        return ERR_NONE;

    return entry->error;
}

/*
 * Check whether a function currently has an error.
 */
bool error_exists(function_id_t function)
{
    return error_get(function) != ERR_NONE;
}

/*
 * Release all error manager resources for this thread.
 */
void error_shutdown(void)
{
    error_entry_t *entry = error_entries;

    while (entry != NULL) {
        error_entry_t *next = entry->next;

        free(entry);
        entry = next;
    }

    error_entries = NULL;

    emergency_error.active = false;
    emergency_error.function = NULL;
}