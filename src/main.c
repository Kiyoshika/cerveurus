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
#include "Parameters.h"

// sample GET callback for /mystr (return the value of a local string)
char* get_str(struct ParameterArray * params, void* user_data)
{
	(void)params;
	return *(char**)user_data;
}

// sample POST callback for /mystr (set the value of a local string)
void set_str(struct ParameterArray * params, void* user_data, char* request_body)
{
	(void)params;

	// Our user_data is an address to a local char* variable.
	// We cast to char** which holds the address and dereference + free before
	// reallocating to the new string from the request_body
	char** data = user_data;
	free(*data);
	*data = strdup(request_body);
}

// make these global so we can access them in the server interrupt
struct ParameterArray * params;
struct Route * route;

// when user closes server with Ctrl+C we need to release the memory and close the socket
void server_interrupt(int sig)
{
	signal(sig, SIG_IGN);
	paramFree(params);
	// TODO: add way to free memory held by route
	exit(0);
}

int main() {
	// enable Ctrl+C interrupt
	signal(SIGINT, &server_interrupt);

	// initiate HTTP_Server
	HTTP_Server http_server;
	init_server(&http_server, 6969);

	// registering Routes
	// TODO: split these into addTemplateRoute and addApiRoute since
	// a template route will never have get/post callbacks (they only return HTML).
	route = initRoute("/", "index.html", NULL, NULL, NULL); 
	addRoute(&route, "/about", "about.html", NULL, NULL, NULL);
	addRoute(&route, "/sth", "sth.html", NULL, NULL, NULL);
	addRoute(&route, "/chicken", "chicken.html", NULL, NULL, NULL);

	// TODO: might need to pass deallocators as well to free user data
	char* my_str = strdup("hello there!");
	addRoute(&route, "/mystr", NULL, &my_str, &get_str, &set_str);

	// create dynamic parameters array
	params = paramInit(10);

	printf("\n====================================\n");
	printf("=========ALL AVAILABLE ROUTES========\n");
	// display all available routes
	inorder(route);

	while (1) {
		char client_msg[4096] = {0};
		char original_msg[4096] = {0};

		int client_socket = accept(http_server.socket, NULL, NULL);

		size_t n_bytes_read = read(client_socket, client_msg, 4095);
		client_msg[n_bytes_read] = '\0';
		memcpy(original_msg, client_msg, n_bytes_read + 1);
		printf("%s\n", client_msg);

		// parsing client socket header to get HTTP method, route
		char *method = "";
		char *urlRoute = "";

		// TODO: cleanup this parsing so we can also fetch Content-Length (or other headers in the future)
		char *client_http_header = strtok(client_msg, "\n");
			
		printf("\n\n%s\n\n", client_http_header);

		char *header_token = strtok(client_http_header, " ");
		
		int header_parse_counter = 0;

		while (header_token != NULL) {

			switch (header_parse_counter) {
				case 0:
					method = header_token;
					break;
				case 1:
					urlRoute = header_token;
					break;
			}
			header_token = strtok(NULL, " ");
			header_parse_counter++;
		}

		printf("The method is %s\n", method);
		printf("The route is %s\n", urlRoute);

		if (strlen(urlRoute) > 0)
		{
			paramParse(params, urlRoute);

			printf("*** PARSED PARAMETERS ***\n");
			for (size_t i = 0; i < params->n_members; ++i)
			{
				printf("KEY: %s\n", params->parameters[i].key);
				printf("VALUE: %s\n\n", params->parameters[i].value);
			}
			// do something with parameters and can clear them to re-use for the next
			// request
			paramClear(params);

			// TODO: abstract the response/set/set/send portion (e.g., http_send_file, http_send_get, http_send_post, etc.)
			char template[100] = "";
			
			// TODO: fix rendering of static files (currently broken..oops)
			if (strstr(urlRoute, "/static/") != NULL)
			{
				strcat(template, "static/index.css");
				http_set_status_code(&http_server, OK);
				send(client_socket, template, strlen(template), 0);
				continue;
			}

			struct Route * destination = search(route, urlRoute);

			if (destination == NULL)
			{
				char* response_data = render_static_file("templates/404.html");
				http_set_status_code(&http_server, NOT_FOUND);
				http_set_response_body(&http_server, response_data);
				send(client_socket, http_server.response, strlen(http_server.response), 0);
				continue;
			}

			if (destination->value)
			{
				strcat(template, "templates/");
				strcat(template, destination->value);
				char* response_data = render_static_file(template);
				http_set_status_code(&http_server, OK);
				http_set_response_body(&http_server, response_data);
				send(client_socket, http_server.response, strlen(http_server.response), 0);
				free(response_data);
			}
			else
			{
				if (strcmp(method, "GET") == 0)
				{
					char* response_data = destination->get_callback(params, destination->user_data);
					http_set_status_code(&http_server, OK);
					http_set_response_body(&http_server, response_data);
					send(client_socket, http_server.response, strlen(http_server.response), 0);
				}
				// TODO: there's a really annoying issue with sending POST requests via cURL/postman
				// where you have to refresh the site 1-2 times before it goes through.
				// Currently have no idea why this happens...
				// Doing the requests using fetch() in javascript are instantaneous
				else if (strcmp(method, "POST") == 0)
				{
					destination->post_callback(params, destination->user_data, original_msg);
					http_set_status_code(&http_server, OK);
					http_set_response_body(&http_server, "");
					send(client_socket, http_server.response, strlen(http_server.response), 0);
				}
			}
		}

		close(client_socket);
	}

	return 0;
}
