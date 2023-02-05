#include "Response.h" // render_static_file()
#include "SQL.h"

// this is really just a wrapper around render_static_file() to
// make it a bit more obvious
char* sql_read(const char* const filename)
{
	return render_static_file(filename);
}

char* sql_find_and_replace(
		const char* const filename,
		const char** const find_text,
		const char** const replace_text)
{
	char* sql_buffer = sql_read(filename);
	if (!sql_buffer)
		return NULL;

	size_t idx = 0;
	while (find_text[idx])
	{
		char* find_text_ptr = NULL;
		if ((find_text_ptr = strstr(sql_buffer, find_text[idx])) != NULL)
		{
			size_t sql_buffer_len = strlen(sql_buffer);
			size_t left_buffer_len = find_text_ptr - sql_buffer;
			size_t new_buffer_len = sql_buffer_len - strlen(find_text[idx]) + strlen(replace_text[idx]);
			char* new_sql_buffer = calloc(new_buffer_len + 1, sizeof(char));
			strncat(new_sql_buffer, sql_buffer, left_buffer_len);
			strcat(new_sql_buffer, replace_text[idx]);
			strcat(new_sql_buffer, &sql_buffer[strlen(find_text[idx]) + left_buffer_len]); 
			void* alloc = realloc(sql_buffer, new_buffer_len + 1);
			if (!alloc)
				return NULL;
			sql_buffer = alloc;
			memset(sql_buffer, 0, new_buffer_len + 1);
			strncat(sql_buffer, new_sql_buffer, new_buffer_len);
			free(new_sql_buffer);
			new_sql_buffer = NULL;
		}
		idx++;
	}

	return sql_buffer;
}
