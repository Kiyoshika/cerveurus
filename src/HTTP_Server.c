#include "HTTP_Server.h"
#include "Routes.h"
#include "Response.h"
#include "SortedArray.h"

#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

// pulled these from https://www.w3.org/Protocols/HTTP/HTRESP.html
const char _status_code_text[8][50] = {
	"HTTP/1.1 200 OK\r\n\r\n",
	"HTTP/1.1 201 CREATED\r\n\r\n",
	"HTTP/1.1 400 Bad request\r\n\r\n",
	"HTTP/1.1 401 Unauthorized\r\n\r\n",
	"HTTP/1.1 403 Forbidden\r\n\r\n",
	"HTTP/1.1 404 Not found\r\n\r\n",
	"HTTP/1.1 500 Internal Error\r\n\r\n",
	"HTTP/1.1 501 Not implemented\r\n\r\n"
};

// maintain a global pointer so we can clean it up when handling interrupt
struct HTTP_Server* http_server_global = NULL;

void http_cleanup(int sig)
{
	signal(sig, SIG_IGN);
	http_free(http_server_global);
	exit(0);
}

void http_init(HTTP_Server * const http_server, int port) {

	// hold a global pointer to server so we can use it in the interrupt
	http_server_global = http_server;

	// create interrupt for server to clean up resources when using Ctrl+C
	signal(SIGINT, http_cleanup);

	http_server->port = port;

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	printf("Binding...\n");
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	printf("Listening...\n");
	listen(server_socket, 50);

	http_server->socket = server_socket;
	http_server->client_socket = -1;

	http_server->response_body = NULL;
	http_server->request_body = NULL;
	http_server->params = sarray_init(10);
	http_server->headers = sarray_init(10);
	http_server->routes = NULL;

	printf("HTTP Server Initialized\nPort: %d\n", http_server->port);
}

static void http_send(
		HTTP_Server* http_server,
		enum http_status_code_e status_code)
{
	http_set_status_code(http_server, status_code);
	http_prepare_response(http_server);
	send(http_server->client_socket, http_server->response_body, strlen(http_server->response_body), 0);

	free(http_server->response_body);
	http_server->response_body = NULL;

	free(http_server->request_body);
	http_server->request_body = NULL;
}

static void http_render(
		HTTP_Server* http_server, 
		struct Route* destination)
{
	char template_path[200] = {0};

	if (destination == NULL)
	{
		http_server->response_body = render_static_file("templates/404.html");
		http_send(http_server, NOT_FOUND);
	}

	// render static template HTML file
	else if (destination->value)
	{
		free(http_server->response_body);
		strcat(template_path, "templates/");
		strcat(template_path, destination->value);
		http_server->response_body = render_static_file(template_path);
		http_send(http_server, OK);
	}
	// handle API response (GET/POST)
	else
	{
		// by default all status codes will return 200 OK
		enum http_status_code_e status_code = OK;
		struct CallbackArgs args = {
			.status_code = &status_code,
			.params = http_server->params,
			.headers = http_server->headers,
			.user_data = destination->user_data,
			.request_body = http_server->request_body
		};

		switch (http_server->request_type)
		{

			case GET:
			{
				http_server->response_body = strdup(destination->get_callback(&args));
				http_send(http_server, status_code);
			}
			break;

			case POST:
			{
				destination->post_callback(&args);
				http_send(http_server, status_code);
			}
			break;

			case DELETE:
			{
				destination->delete_callback(&args);
				http_send(http_server, status_code);
			}
			break;
			
			default:
				printf("\n\n==== WARNING ====\nUnsupported request type.");
				break;
		}
	}
}

static void http_parse_query_params(
		HTTP_Server* http_server)
{
	char* url_copy = strdup(http_server->request_url);
	memset(http_server->request_url, 0, strlen(http_server->request_url));

	// temporary large buffers to record the key/values before passing
	// them to sarray_add
	char key[1024] = {0};
	char value[1024] = {0};

	/*
	 * Parsing Rule:
	 * k = keys
	 * v = values
	 */
	char parsing_rule = 0;

	for (size_t i = 0; i < strlen(url_copy); ++i)
	{
		switch (url_copy[i])
		{
			case '?':
				parsing_rule = 'k';
				continue;
			case '=':
				parsing_rule = 'v';
				continue;
			case '&':
				sarray_add(http_server->params, key, value);
				memset(key, 0, 1024);
				memset(value, 0, 1024);
				parsing_rule = 'k';
				continue;
			default:
				if (parsing_rule == 0)
					strncat(http_server->request_url, &url_copy[i], 1);
				break;
		}

		switch (parsing_rule)
		{
			case 'k':
				strncat(key, &url_copy[i], 1);
				break;
			case 'v':
				strncat(value, &url_copy[i], 1);
				break;
		}
	}

	sarray_add(http_server->params, key, value);
	free(url_copy);
}

static void http_parse_client_data(
		HTTP_Server* http_server,
		char* client_data)
{
	char* header_line = NULL;

	header_line = strtok(client_data, "\n");

	// on first line, first token is request type (GET, POST, etc.)
	char token[HTTP_URL_LEN] = {0};
	size_t token_idx = 0;
	bool parsed_request_type = false;
	for (size_t i = 0; i < strlen(header_line); ++i)
	{
		if (header_line[i] == ' ')
		{
			if (!parsed_request_type)
			{
				parsed_request_type = true;
				if (strcmp(token, "GET") == 0)
					http_server->request_type = GET;
				else if (strcmp(token, "POST") == 0)
					http_server->request_type = POST;
				else if (strcmp(token, "DELETE") == 0)
					http_server->request_type = DELETE;
				else
					printf("\nWARNING: Unsupported request type '%s'\n", token);
			}
			else
				strncat(http_server->request_url, token, HTTP_URL_LEN);

			token_idx = 0;
			memset(token, 0, HTTP_URL_LEN);
		}
		else
			token[token_idx++] = header_line[i];
		
	}

	// parse query parameters from URL (if any)
	http_parse_query_params(http_server);

	// iterate through rest of headers
	// until we hit empty token, then the following token
	// after that is the request body
	while ((header_line = strtok(NULL, "\n")) != NULL)
	{
		// if header line is blank space, that means next header line
		// is request body
		if (strlen(header_line) == 1)
		{
			header_line = strtok(NULL, "\n");
			if (header_line) // if request body is present
			{
				// convert string to size_t for content length
				struct KeyValuePair* find = sarray_get(http_server->headers, "Content-Length");
				char* ptr;
				size_t length = strtoul(find->value, &ptr, 10);

				http_server->request_body = calloc(length + 1, sizeof(char));
				strncat(http_server->request_body, header_line, length);
			}
			break;
		}

		printf("HEADER LINE: %s\n", header_line);
		char header_key[4096] = {0};
		char header_value[4096] = {0};

		bool parsing_key = true;
		for (size_t i = 0; i < strlen(header_line); ++i)
		{
			switch (header_line[i])
			{
				case ':':
					if (parsing_key)
					{
						parsing_key = false;
						i++;
						continue;
					}
				break;

				default:
					if (parsing_key)
						strncat(header_key, &header_line[i], 1);
					else
						strncat(header_value, &header_line[i], 1);
					break;
			}
		}
		sarray_add(http_server->headers, header_key, header_value);
	}
}

void http_listen(HTTP_Server* http_server)
{
	char client_data[4096] = {0};

	while (1)
	{
		memset(http_server->request_url, 0, HTTP_URL_LEN);
		memset(client_data, 0, 4096);

		printf("Accepting...\n");
		
		http_server->client_socket = accept(http_server->socket, NULL, NULL);

		printf("Accepted!\n\n");

		// read client data (headers + request body)
		ssize_t n_bytes_read = read(http_server->client_socket, client_data, 4095);

		if (n_bytes_read > 0)
		{
			client_data[n_bytes_read] = '\0';

			// parse headers and response body
			http_parse_client_data(http_server, client_data);

			// fetch route
			struct Route* destination = search(http_server->routes, http_server->request_url);
			http_render(http_server, destination);

			// cleanup URL parameters for next request
			sarray_clear(http_server->params);

			close(http_server->client_socket);
		}

	}
}

void http_set_status_code(
		HTTP_Server* const http_server,
		const enum http_status_code_e status_code)
{
	memset(http_server->status_code, 0, HTTP_STATUS_CODE_LEN);
	const char* _status = _status_code_text[status_code];
	memcpy(http_server->status_code, _status, strlen(_status));
}

void http_prepare_response(
		HTTP_Server* const http_server)
{
	if (http_server->response_body)
	{
		// allocate enough memory for response
		char* response_data = strdup(http_server->response_body);
		free(http_server->response_body);
		http_server->response_body = calloc(strlen(http_server->status_code) + strlen(response_data) + 1, sizeof(char));
		if (!http_server->response_body)
		{
			// TODO: throw some type of error here
		}
		// prepend the status code before adding the body
		strncat(http_server->response_body, http_server->status_code, strlen(http_server->status_code));

		// add response body (while accounting for the space we already took up by the response code
		// NOTE: the -1 is for the null terminator
		size_t max_len = (size_t)HTTP_RESPONSE_BODY_LEN - strlen(http_server->status_code) - 1;
		strncat(http_server->response_body, response_data, max_len);
		free(response_data);
		response_data = NULL;
	}
	else
		http_server->response_body = strdup(http_server->status_code);
}

void http_add_route_template(
		HTTP_Server* const http_server,
		char* route_path,
		char* template_file_name)
{
	if (!http_server->routes)
		http_server->routes = initRoute(route_path, template_file_name, NULL, NULL, NULL, NULL, NULL);
	else
		addRoute(&http_server->routes, route_path, template_file_name, NULL, NULL, NULL, NULL, NULL);
}

void http_add_route_api(
		HTTP_Server* const http_server,
		char* route_path,
		void* user_data,
		void (*user_data_dealloc)(void* user_data),
		char* (*get_callback)(struct CallbackArgs * const args),
		void (*post_callback)(struct CallbackArgs * const args),
		void (*delete_callback)(struct CallbackArgs * const args))
{
	if (!http_server->routes)
		http_server->routes = initRoute(route_path, NULL, user_data, user_data_dealloc,  get_callback, post_callback, delete_callback);
	else
		addRoute(&http_server->routes, route_path, NULL, user_data, user_data_dealloc, get_callback, post_callback, delete_callback);
}

void http_free(HTTP_Server* const http_server)
{
	if (http_server->response_body)
		free(http_server->response_body);

	if (http_server->request_body)
		free(http_server->request_body);

	if (http_server->params)
		sarray_free(&http_server->params);

	if (http_server->headers)
		sarray_free(&http_server->headers);

	if (http_server->routes)
		freeRoutes(&http_server->routes);
	free(http_server->routes);

	// NOTE: the HTTP server itself is not heap-allocated
	// so we don't need to free(http_server)
}
