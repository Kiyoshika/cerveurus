#ifndef ROUTES_H
#define ROUTES_H

#include <stdlib.h>
#include <string.h>

// forward declaration
struct SortedArray;

struct Route {
	char* key;
	char* value;
	void* user_data;

	char* (*get_callback)(
			struct SortedArray * sarray, 
			void* user_data);
	void (*post_callback)(
			struct SortedArray * sarray,
			void* user_data,
			char* request_body);

	struct Route *left, *right;
};

struct Route * initRoute(
		char* key, 
		char* value,
		void* user_data,
		char* (*get_callback)(struct SortedArray*, void*),
		void (*post_callback)(struct SortedArray*, void*, char*));

void addRoute(
		struct Route ** root, 
		char* key, 
		char* value,
		void* user_data,
		char* (*get_callback)(struct SortedArray*, void*),
		void (*post_callback)(struct SortedArray*, void*, char*));

struct Route * search(struct Route * root, char * key);

void inorder(struct Route * root );

#endif
