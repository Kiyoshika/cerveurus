#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "HTTP_Server.h"
#include "Routes.h"
#include "Response.h"
#include "SortedArray.h"

// sample GET callback for /mystr (return the value of a local string)
char* get_str(struct SortedArray * params, struct SortedArray * headers, void* user_data)
{
	(void)params;
	(void)headers;
	return *(char**)user_data;
}

// sample POST callback for /mystr (set the value of a local string)
void set_str(struct SortedArray * params, struct SortedArray * headers, void* user_data, char* request_body)
{
	(void)params;
	(void)headers;

	// Our user_data is an address to a local char* variable.
	// We cast to char** which holds the address and dereference + free before
	// reallocating to the new string from the request_body
	char** data = user_data;
	free(*data);
	*data = strdup(request_body);

	/* Example of how to pull a header value by key

	struct KeyValuePair* find = sarray_get(headers, "MyHeader");
	*data = strdup(find->value);

	*/
}

// sample DELETE callback for /mystr that just sets it to blank string
void delete_str(struct SortedArray * params, struct SortedArray * headers, void* user_data, char* request_body)
{
	(void)params;
	(void)headers;
	(void)request_body;

	char** data = user_data;
	free(*data);
	*data = strdup("");
}

void dealloc(void* user_data)
{
	char** data = user_data;
	free(*data);
	*data = NULL;
}

int main() {
	// initiate HTTP_Server
	HTTP_Server http_server;
	http_init(&http_server, 6969);

	// registering Routes
	// a template route will never have get/post callbacks (they only return HTML).
	// by default these will search the templates/ folder
	http_add_route_template(&http_server, "/", "index.html");
	http_add_route_template(&http_server, "/about", "about.html");
	http_add_route_template(&http_server, "/sth", "sth.html");
	http_add_route_template(&http_server, "/chicken", "chicken.html");

	// this route manages a string resource. We also pass a deallocator
	// that the server can clean up when it's killed (e.g., Ctrl+C)
	// NOTE that you don't need to provide ALL callbacks, only the ones you want.
	char* my_str = strdup("hello there!");
	http_add_route_api(&http_server, "/mystr", &my_str, &dealloc, &get_str, &set_str, &delete_str);

	printf("\n====================================\n");
	printf("=========ALL AVAILABLE ROUTES========\n");
	// display all available routes
	inorder(http_server.routes);

	http_listen(&http_server);

	return 0;
}
