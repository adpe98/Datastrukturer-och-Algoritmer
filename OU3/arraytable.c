#include <stdlib.h>
#include <stdbool.h>
#include "table.h"
#include "array_1d.h"

// The maximum allowed size of the field is set to 8000 characthers
#define MAX_TABLE_SIZE 80000

/*
//Each table entry has a value and key and because is a
//void pointer it can be any type of data.
*/
struct table_entry
{
    void *key;
    void *value;
};

/*
//entrites is a field of table entry structs. The field
//have a maximum size of MAX_TABLE_SIZE.
//
//size keeps track of the number of entries in the table
*/
struct table
{
    array_1d *entries;
    compare_function *key_cmp_func;
    free_function key_free_func;
    free_function value_free_func;
};

/**
 * table_empty() - Create an empty table.
 * @key_cmp_func: A pointer to a function to be used to compare keys.
 * @key_free_func: A pointer to a function (or NULL) to be called to
 *		   de-allocate memory for keys on remove/kill.
 * @value_free_func: A pointer to a function (or NULL) to be called to
 *		     de-allocate memory for values on remove/kill.
 *
 * Returns: Pointer to a new table.
 */
table *table_empty(compare_function key_cmp_func, free_function key_free_func, free_function value_free_func)
{
    // Allocate memory for the table structure.
    table *t = malloc(sizeof(table));

    // Initialize the table with the provided function pointers.
    t->key_cmp_func = key_cmp_func;
    t->key_free_func = key_free_func;
    t->value_free_func = value_free_func;

    // Create an empty array_1d for the table entries.
    t->entries = array_1d_create(0, MAX_TABLE_SIZE, NULL);

    return t;
}

/**
 * table_is_empty() - Check if a table is empty.
 * @table: Table to check.
 *
 * Returns: True if table contains no key/value pairs, false otherwise.
 */
bool table_is_empty(const table *t)
{
    // Iterate through the array.
    for (int position = array_1d_low(t->entries); position <= array_1d_high(t->entries); position++)
    {
        // If an element with a value is found, the table is not empty.
        if (array_1d_has_value(t->entries, position))
        {
            return false;
        }
    }

    // If the loop completes without finding any elements with values, the table is empty.
    return true;
}

/**
 * table_insert() - Add a key/value pair to a table.
 * @table: Table to manipulate.
 * @key: A pointer to the key value.
 * @value: A pointer to the value value.
 *
 * Insert the key/value pair into the table. No test is performed to
 * check if key is a duplicate. table_lookup() will return the latest
 * added value for a duplicate key. table_remove() will remove all
 * duplicates for a given key.
 *
 * Returns: Nothing.
 */
void table_insert(table *t, void *key, void *value)
{
    struct table_entry *entry = malloc(sizeof(struct table_entry));

    entry->key = key;
    entry->value = value;

    int position = 0;

    while (array_1d_has_value(t->entries, position))
    {
        struct table_entry *entry_temp = array_1d_inspect_value(t->entries, position);

        if (t->key_cmp_func(entry_temp->key, key) == 0)
        {
            array_1d_set_value(t->entries, entry, position);

            if (t->key_free_func != NULL)
            {
                t->key_free_func(entry_temp->key);
            }
            if (t->value_free_func != NULL)
            {
                t->value_free_func(entry_temp->value);
            }

            free(entry_temp);
            return;
        }
        position++;
    }

    entry->key = key;
    entry->value = value;
    array_1d_set_value(t->entries, entry, position);
}

/**
 * table_lookup() - Look up a given key in a table.
 * @table: Table to inspect.
 * @key: Key to look up.
 *
 * Returns: The value corresponding to a given key, or NULL if the key
 * is not found in the table. If the table contains duplicate keys,
 * the value that was latest inserted will be returned.
 */
void *table_lookup(const table *t, const void *key)
{
    int position = 0;
    void *latest_value = NULL;

    // Iterate over the array to find the latest inserted value for the key.
    while (array_1d_has_value(t->entries, position))
    {
        struct table_entry *entry = array_1d_inspect_value(t->entries, position);

        if (t->key_cmp_func(entry->key, key) == 0)
        {
            latest_value = entry->value;
        }
        position++;
    }

    return latest_value;
}

/**
 * table_choose_key() - Return an arbitrary key.
 * @t: Table to inspect.
 *
 * Return an arbitrary key stored in the table. Can be used together
 * with table_remove() to deconstruct the table. Undefined for an
 * empty table.
 *
 * Returns: An arbitrary key stored in the table.
 */

void *table_choose_key(const table *t)
{
    // Start at the beginning of the array.
    int position = array_1d_low(t->entries);

    // Iterate over the array to find the first non-null entry.
    while (position <= array_1d_high(t->entries))
    {
        if (array_1d_has_value(t->entries, position))
        {
            // Inspect the table entry.
            struct table_entry *entry = array_1d_inspect_value(t->entries, position);

            // Return the key of the first non-null entry.
            return entry->key;
        }
        // Move on to the next position in the array.
        position++;
    }

    // If no non-null entries are found, return NULL.
    return NULL;
}

/*
table_remove() - Remove a key/value pair in the table.
@table: Table to manipulate.
@key: Key for which to remove pair.
Any matching duplicates will be removed. Will call any free
functions set for keys/values. Does nothing if key is not found in
the table.
Returns: Nothing.
*/
void table_remove(table *t, const void *key)
{
    void *deferred_ptr = NULL;

    int pos = 0;
    int removed_pos = -1;

    while (array_1d_has_value(t->entries, pos))
    {
        struct table_entry *entry = array_1d_inspect_value(t->entries, pos);

        if (t->key_cmp_func(entry->key, key) == 0)
        {
            if (t->key_free_func != NULL)
            {
                if (entry->key == key)
                {
                    deferred_ptr = entry->key;
                }
                else
                {
                    t->key_free_func(entry->key);
                }
            }
            if (t->value_free_func != NULL)
            {
                t->value_free_func(entry->value);
            }

            removed_pos = pos;
            array_1d_set_value(t->entries, NULL, pos);
            free(entry);
        }
        else
        {
            if (removed_pos != -1)
            {
                array_1d_set_value(t->entries, entry, removed_pos);
                array_1d_set_value(t->entries, NULL, pos);
                removed_pos++;
            }
        }
        pos++;
    }

    if (deferred_ptr != NULL)
    {
        t->key_free_func(deferred_ptr);
    }
}

/*
 * table_kill() - Destroy a table.
 * @table: Table to destroy.
 *
 * Return all dynamic memory used by the table and its elements. If a
 * free_func was registered for keys and/or values at table creation,
 * it is called each element to free any user-allocated memory
 * occupied by the element values.
 *
 * Returns: Nothing.
 */
void table_kill(table *t)
{

    int position = array_1d_low(t->entries);

    while (position <= array_1d_high(t->entries))
    {
        if (array_1d_has_value(t->entries, position))
        {
            struct table_entry *entry = array_1d_inspect_value(t->entries, position);
            if (t->key_free_func != NULL)
            {
                t->key_free_func(entry->key);
            }
            if (t->value_free_func != NULL)
            {
                t->value_free_func(entry->value);
            }
            free(entry);
        }
        position++;
    }

    array_1d_kill(t->entries);
    free(t);
}

/**
 * table_print() - Print the given table.
 * @t: Table to print.
 * @print_func: Function called for each key/value pair in the table.
 *
 * Iterates over the key/value pairs in the table and prints them.
 * Will print all stored elements, including duplicates.
 *
 * Returns: Nothing.
 */
void table_print(const table *t, inspect_callback_pair print_func)
{
    int position = array_1d_low(t->entries);

    while (position <= array_1d_high(t->entries))
    {
        if (array_1d_has_value(t->entries, position))
        {
            struct table_entry *y = array_1d_inspect_value(t->entries, position);
            print_func(y->key, y->value);
        }
        position++;
    }
}