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
char* get_str(struct SortedArray * params, void* user_data)
{
	(void)params;
	return *(char**)user_data;
}

// sample POST callback for /mystr (set the value of a local string)
void set_str(struct SortedArray * params, void* user_data, char* request_body)
{
	(void)params;

	// Our user_data is an address to a local char* variable.
	// We cast to char** which holds the address and dereference + free before
	// reallocating to the new string from the request_body
	char** data = user_data;
	free(*data);
	*data = strdup(request_body);
}

int main() {
	// initiate HTTP_Server
	HTTP_Server http_server;
	http_init(&http_server, 6969);

	// registering Routes
	// TODO: move adding routes into HTTP server (e.g., http_add_route())
	// TODO: split these into addTemplateRoute and addApiRoute since
	// a template route will never have get/post callbacks (they only return HTML).
	http_server.routes = initRoute("/", "index.html", NULL, NULL, NULL); 
	addRoute(&http_server.routes, "/about", "about.html", NULL, NULL, NULL);
	addRoute(&http_server.routes, "/sth", "sth.html", NULL, NULL, NULL);
	addRoute(&http_server.routes, "/chicken", "chicken.html", NULL, NULL, NULL);

	// TODO: might need to pass deallocators as well to free user data
	char* my_str = strdup("hello there!");
	addRoute(&http_server.routes, "/mystr", NULL, &my_str, &get_str, &set_str);

	printf("\n====================================\n");
	printf("=========ALL AVAILABLE ROUTES========\n");
	// display all available routes
	inorder(http_server.routes);

	http_listen(&http_server);

	return 0;
}
