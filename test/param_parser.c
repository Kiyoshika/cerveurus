#include "Parameters.h"
#include <stdio.h>

/*
 * Just testing the functionality locally so I don't have to
 * keep running/killing the server over and over.
 *
 * These could be converted to "proper" tests but for now
 * they are purely visual.
 */
int main()
{
	char* url = strdup("/someNested/route?name=Zach&val=100&jimmy=John");
	struct ParameterArray * params = paramInit(10);
	paramParse(params, url);

	int result = 0;

	// validate that URL is cleaned up
	if (strcmp(url, "/someNested/route") != 0)
	{
		fprintf(stderr, "\nExpected URL to be: %s\nBut instead got: %s\n", "/someNested/route", url);
		result = -1;
		goto cleanup;
	}

	// could wrap these checks in a function but I'm too lazy now
	
	struct Parameter* search = NULL;

	search = paramGet(params, "name");
	printf("FOUND VALUE: %s\n\n", search->value);
	if (strcmp("Zach", search->value) != 0)
	{
		fprintf(stderr, "\nExpected to find '%s' for key '%s' but got '%s' instead.\n",
				"Zach",
				"name",
				search->value);
		result = -1;
		goto cleanup;
	}

	search = paramGet(params, "val");
	if (strcmp("100", search->value) != 0)
	{
		fprintf(stderr, "\nExpected to find '%s' for key '%s' but got '%s' instead.\n",
				"100",
				"val",
				search->value);
		result = -1;
		goto cleanup;
	}

	search = paramGet(params, "xyz");
	if (search != NULL)
	{
		fprintf(stderr, "\nExpected to find empty value for key '%s' but got '%s' instead.\n",
				"xyz",
				search->value);
		result = -1;
		goto cleanup;
	}

cleanup:
	paramFree(params);
	free(url);
	return result;
}
