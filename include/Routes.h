#ifndef ROUTES_H
#define ROUTES_H

#include <stdlib.h>
#include <string.h>

// I need the enums defined in here
#include "HTTP_Server.h"

// forward declaration
struct SortedArray;

struct Route {
	char* key;
	char* value;

	void* user_data;
	void (*user_data_dealloc)(void*);

	char* (*get_callback)(
			enum http_status_code_e* status_code,
			struct SortedArray * params, 
			struct SortedArray * headers,
			void* user_data);

	void (*post_callback)(
			enum http_status_code_e* status_code,
			struct SortedArray * params,
			struct SortedArray * headers,
			void* user_data,
			char* request_body);

	void (*delete_callback)(
			enum http_status_code_e* status_code,
			struct SortedArray * params,
			struct SortedArray * headers,
			void* user_data,
			char* request_body);

	struct Route *left, *right;
};
struct Route * initRoute(
		char* key, 
		char* value,
		void* user_data,
		void (*user_data_dealloc)(void*),
		char* (*get_callback)(enum http_status_code_e*, struct SortedArray*, struct SortedArray*, void*),
		void (*post_callback)(enum http_status_code_e*, struct SortedArray*, struct SortedArray*, void*, char*),
		void (*delete_callback)(enum http_status_code_e*, struct SortedArray*, struct SortedArray*, void*, char*));

void addRoute(
		struct Route ** root, 
		char* key, 
		char* value,
		void* user_data,
		void (*user_data_dealloc)(void*),
		char* (*get_callback)(enum http_status_code_e*, struct SortedArray*, struct SortedArray*, void*),
		void (*post_callback)(enum http_status_code_e*, struct SortedArray*, struct SortedArray*, void*, char*),
		void (*delete_callback)(enum http_status_code_e*, struct SortedArray*, struct SortedArray*, void*, char*));

struct Route * search(
		struct Route * root, 
		char * key);

void inorder(const struct Route * const root );

void freeRoutes(struct Route ** root);

#endif
