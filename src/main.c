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

// each callback (GET/POST/DELETE) defaults to return 200 OK.
// this can be changed by adjusting the pointer, e.g., args->status_code = CREATED
// see HTTP_Server.h for list of status codes

// sample GET callback for /mystr (return the value of a local string)
char* get_str(struct CallbackArgs * const args)
{
	return *(char**)args->user_data;
}

// sample POST callback for /mystr (set the value of a local string)
void set_str(struct CallbackArgs * const args)
{
	// Our user_data is an address to a local char* variable.
	// We cast to char** which holds the address and dereference + free before
	// reallocating to the new string from the request_body
	char** data = args->user_data;
	free(*data);
	*data = strdup(args->request_body);

	/* Example of how to pull a header value by key

	struct KeyValuePair* find = sarray_get(args->headers, "MyHeader");
	*data = strdup(find->value);

	*/
}

// sample DELETE callback for /mystr that just sets it to blank string
void delete_str(struct CallbackArgs * const args)
{
	char** data = args->user_data;
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
	http_init(&http_server, 6100);

	// registering Routes
	// a template route will never have get/post callbacks (they only return HTML).
	// by default these will search the templates/ folder
	http_add_route_template(&http_server, "/", "index.html");
	http_add_route_template(&http_server, "/about", "about.html");
	http_add_route_template(&http_server, "/sth", "sth.html");
	http_add_route_template(&http_server, "/chicken", "chicken.html");

	// registering static files (css/js files)
	// this will search the static/ folder which we can reference in HTML like
	// <link rel = "stylesheet" href = "static/index.css"/>
	http_add_route_static(&http_server, "/static/index.css");
	http_add_route_static(&http_server, "/static/index2.css"); // used in "/about" page

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
