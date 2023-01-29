#include "SortedArray.h"

static void _print_critical()
{
	printf("============ CRITICAL ============\n");
	printf("Memory could not be allocated for sorted array.\n");
}

struct SortedArray * sarray_init(const size_t capacity)
{
	struct SortedArray * sarray = malloc(sizeof(struct SortedArray));

	if (!sarray)
	{
		_print_critical();
		return NULL;
	}

	sarray->item = calloc(capacity, sizeof(struct KeyValuePair));
	
	if (!sarray->item)
	{
		_print_critical();
		return NULL;
	}

	sarray->is_sorted = false;
	sarray->n_members = 0;
	sarray->capacity = capacity;

	for (size_t i = 0; i < capacity; ++i)
	{
		sarray->item[i].key = NULL;
		sarray->item[i].value = NULL;
	}

	return sarray;
}

bool sarray_add(
		struct SortedArray * sarray,
		const char* key,
		const char* value)
{
	const size_t i = sarray->n_members;

	sarray->item[i].key = strdup(key);
	sarray->item[i].value = strdup(value);

	sarray->is_sorted = false;
	sarray->n_members++;

	// follow doubling rule
	if (sarray->n_members == sarray->capacity)
	{
		void* alloc = realloc(sarray->item, sarray->capacity * 2 * sizeof(struct KeyValuePair));
		if (!alloc)
		{
			_print_critical();
			return false;
		}

		sarray->capacity *= 2;
		sarray->item = alloc;
	}

	return true;
}

static int key_cmp(const void* a, const void* b)
{
	const struct KeyValuePair * _a = a;
	const struct KeyValuePair * _b = b;
	
	return strcmp(_a->key, _b->key);
}

void sarray_sort(struct SortedArray * sarray)
{
	qsort(sarray->item, sarray->n_members, sizeof(struct KeyValuePair), &key_cmp);
	sarray->is_sorted = true;
}

struct KeyValuePair * sarray_get(
		struct SortedArray * sarray,
		const char* key)
{
	if (!sarray->is_sorted)
		sarray_sort(sarray);

	struct KeyValuePair search_key;
	search_key.key = (char*)key; // try to silent warning about losing const-ness
	
	return bsearch(&search_key, sarray->item, sarray->n_members, sizeof(struct KeyValuePair), &key_cmp);

}

void sarray_clear(struct SortedArray * sarray)
{
	for (size_t i = 0; i < sarray->n_members; ++i)
	{
		free(sarray->item[i].key);
		sarray->item[i].key = NULL;

		free(sarray->item[i].value);
		sarray->item[i].value = NULL;
	}

	sarray->n_members = 0;
}

void sarray_free(struct SortedArray ** sarray)
{
	sarray_clear(*sarray);
	free((*sarray)->item);
	(*sarray)->item = NULL;
	free(*sarray);
	*sarray = NULL;
}
