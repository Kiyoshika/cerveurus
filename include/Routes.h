#ifndef ROUTES_H
#define ROUTES_H

#include <stdlib.h>
#include <string.h>

// I need the enums defined in here
#include "HTTP_Server.h"

// forward declaration
struct SortedArray;

// arguments passed to GET/POST/DELETE/etc. callback functions
// that end user can reference when handling API requests.
struct CallbackArgs
{
	char* url;
	enum http_status_code_e* status_code;
	const struct SortedArray * const params;
	const struct SortedArray * const headers;
	void* user_data;
	const char* const request_body;
};

struct Route
{
	char* key;
	char* value;

	void* user_data;
	void (*user_data_dealloc)(void*);

	char* (*get_callback)(struct CallbackArgs * const args);
	void (*post_callback)(struct CallbackArgs * const args);
	void (*delete_callback)(struct CallbackArgs * const args);
			
	struct Route *left, *right;
};

struct Route * initRoute(
		char* key, 
		char* value,
		void* user_data,
		void (*user_data_dealloc)(void*),
		char* (*get_callback)(struct CallbackArgs * const args),
		void (*post_callback)(struct CallbackArgs * const args),
		void (*delete_callback)(struct CallbackArgs * const args));

void addRoute(
		struct Route ** root, 
		char* key, 
		char* value,
		void* user_data,
		void (*user_data_dealloc)(void*),
		char* (*get_callback)(struct CallbackArgs * const args),
		void (*post_callback)(struct CallbackArgs * const args),
		void (*delete_callback)(struct CallbackArgs * const args));

struct Route * search(
		struct Route * root, 
		char * key);

void inorder(const struct Route * const root );

void freeRoutes(struct Route ** root);

#endif
