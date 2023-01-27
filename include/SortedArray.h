#ifndef SORTED_ARRAY_H
#define SORTED_ARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct KeyValuePair
{
	char* key;
	char* value;
};

struct SortedArray
{
	struct KeyValuePair * item;
	bool is_sorted;
	size_t n_members;
	size_t capacity;
};

struct SortedArray * sarray_init(const size_t capacity);

// Parse parameters from a string (e.g., when receiving ROUTE url).
// Takes an existing Parameter pointer and adds new parameters parsed from string.
// This will MODIFY the url inplace to remove all the parameter text before it's passed
// on to render a static template.
void paramParse(
		struct SortedArray * sarray, 
		char* url);

// Clear all keys & values and set n_members back to zero.
// This allows us to reuse the struct for multiple requests.
void sarray_clear(struct SortedArray * sarray);

bool sarray_add(
		struct SortedArray * sarray,
		const char* key,
		const char* value);

// Sort by keys ascending. This is primarily used internally
// but can be used externally if needed for whatever reason.
void sarray_sort(struct SortedArray * sarray);

// Return a pointer to a KeyValuePair with a given key.
// Any modifications done will be reflected in array.
struct KeyValuePair * sarray_get(
		struct SortedArray * sarray,
		const char* key);

void sarray_free(struct SortedArray * sarray);

#endif
