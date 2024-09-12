#ifndef corelox_table_h
#define corelox_table_h

#include "common.h"
#include "value.h"

/**
 * @file table.h 
 * @brief Defines a hash table for storing key-value pairs.
 * 
 * Details of the implementation are as follows:
 * 
 * - Hash Function: FNV-1a (Fowler-Noll-Vo alternate)
 * - Collision Resolution: Open Addressing (Linear Probing)
 * - Load Factor: 0.75 (change TABLE_MAX_LOAD to adjust)
 * - Growth Policy: Double the capacity when load factor is reached
 * - Deletion strategy: Tombstones (marking deleted entries)
 */

/**
 * @brief Represents a key-value pair in the hash table.
 * 
 * The `Entry` struct represents a single key-value pair in the hash table.
 * Each entry contains a key (an `ObjString` pointer) and a value (a `Value`).
 */
typedef struct {
  ObjString* key;
  Value value;
} Entry;

/**
 * @brief Represents a hash table for storing key-value pairs.
 * 
 * The `Table` struct represents a hash table that stores key-value pairs.
 * The table is implemented as an array of `Entry` structs, with each entry
 * containing a key (an `ObjString` pointer) and a value (a `Value`).
 */
typedef struct {
  int count;
  int capacity;
  Entry* entries;
} Table;

/**
 * @brief Initializes a new hash table.
 * 
 * This function initializes a new hash table with the default capacity.
 * The table is allocated on the heap and its fields are initialized.
 * 
 * @param table A pointer to the hash table to initialize.
 */
void initTable(Table* table);

/**
 * @brief Frees the memory allocated for a hash table.
 * 
 * This function frees the memory allocated for a hash table, including
 * the array of entries and the table itself. It is called when the table
 * is no longer needed to prevent memory leaks.
 * 
 * @param table A pointer to the hash table to free.
 */
void freeTable(Table* table);

/**
 * @brief Retrieves a value from the hash table by key.
 * 
 * This function retrieves a value from the hash table by key. It searches
 * for the key in the table and, if found, returns the corresponding value.
 * 
 * @param table A pointer to the hash table to search.
 * @param key The key to search for in the table.
 * @param value A pointer to store the value if the key is found.
 * @return true if the key is found in the table, false otherwise.
 */
bool tableGet(Table* table, ObjString* key, Value* value);

/**
 * @brief Finds a string in the hash table by its characters.
 * 
 * This function searches for a string in the hash table by its characters.
 * It computes the hash value of the string and looks for a matching key
 * in the table. If a matching key is found, the corresponding `ObjString`
 * pointer is returned.
 * 
 * @param table A pointer to the hash table to search.
 * @param chars The characters of the string to search for.
 * @param length The length of the string.
 * @param hash The hash value of the string.
 * @return The `ObjString` pointer if the string is found, NULL otherwise.
 */
ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash);

/**
 * @brief Sets a key-value pair in the hash table.
 * 
 * This function sets a key-value pair in the hash table. It searches for
 * the key in the table and, if found, updates the corresponding value. If
 * the key is not found, a new entry is added to the table with the key-value
 * pair. The function returns true if the key was added or updated successfully.
 * 
 * @param table A pointer to the hash table to update.
 * @param key The key to set in the table.
 * @param value The value to associate with the key.
 * @return true if the key was added or updated, false otherwise.
 */
bool tableSet(Table* table, ObjString* key, Value value);

/**
 * @brief Removes a key-value pair from the hash table.
 * 
 * This function removes a key-value pair from the hash table by key. It
 * searches for the key in the table and, if found, removes the corresponding
 * entry (by replacing it with a tombstone). 
 * The function returns true if the key was found and removed successfully.
 * 
 * @param table A pointer to the hash table to update.
 * @param key The key to remove from the table.
 * @return true if the key was removed, false otherwise.
 */
bool tableDelete(Table* table, ObjString* key);

/**
 * @brief Adds all key-value pairs from one table to another.
 * 
 * This function adds all key-value pairs from one table to another. It
 * iterates over the entries in the source table and adds each key-value
 * pair to the destination table. If a key already exists in the destination
 * table, the corresponding value is updated.
 * 
 * @param from The source table to copy key-value pairs from.
 * @param to The destination table to copy key-value pairs to.
 */
void tableAddAll(Table* from, Table* to);

#endif