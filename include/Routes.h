#ifndef ROUTES_H
#define ROUTES_H

#include <stdlib.h>
#include <string.h>

// forward declaration
struct ParameterArray;

struct Route {
	char* key;
	char* value;
	void* user_data;

	char* (*get_callback)(
			struct ParameterArray * params, 
			void* user_data);
	void (*post_callback)(
			struct ParameterArray * params,
			void* user_data,
			char* request_body);

	struct Route *left, *right;
};

struct Route * initRoute(
		char* key, 
		char* value,
		void* user_data,
		char* (*get_callback)(struct ParameterArray*, void*),
		void (*post_callback)(struct ParameterArray*, void*, char*));

void addRoute(
		struct Route ** root, 
		char* key, 
		char* value,
		void* user_data,
		char* (*get_callback)(struct ParameterArray*, void*),
		void (*post_callback)(struct ParameterArray*, void*, char*));

struct Route * search(struct Route * root, char * key);

void inorder(struct Route * root );

#endif
