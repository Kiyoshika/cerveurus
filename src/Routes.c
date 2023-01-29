#include "Routes.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Route * initRoute(
		char* key, 
		char* value,
		void* user_data,
		void (*user_data_dealloc)(void*),
		char* (*get_callback)(
			struct SortedArray * params, 
			struct SortedArray * headers, 
			void* user_data),
		void (*post_callback)(
			struct SortedArray * params, 
			struct SortedArray * headers,
			void* user_data, char* request_body))
{
	struct Route * temp = (struct Route *) malloc(sizeof(struct Route));

	temp->key =  key;
	temp->value = value;
	temp->get_callback = get_callback;
	temp->post_callback = post_callback;
	temp->user_data = user_data;
	temp->user_data_dealloc = user_data_dealloc;

	temp->left = temp->right = NULL;
	return temp;
}

void inorder(const struct Route* const root)
{

    if (root != NULL)
	{
        inorder(root->left);
        printf("%s -> %s \n", root->key, root->value);
        inorder(root->right);
    }
}

void freeRoutes(struct Route** root)
{
	if (*root != NULL)
	{
		if ((*root)->left)
			freeRoutes(&(*root)->left);

		if ((*root)->right)
			freeRoutes(&(*root)->right);

		if ((*root)->user_data_dealloc)
			(*root)->user_data_dealloc((*root)->user_data);

		free(*root);
		*root = NULL;
	}
}

void addRoute(
		struct Route ** root, 
		char* key, 
		char* value,
		void* user_data,
		void (*user_data_dealloc)(void* user_data),
		char* (*get_callback)(
			struct SortedArray * params, 
			struct SortedArray * headers,
			void* user_data),
		void (*post_callback)(
			struct SortedArray * params, 
			struct SortedArray * headers,
			void* user_data, char* request_body)) {
	if (*root == NULL) {
		*root = initRoute(key, value, user_data, user_data_dealloc, get_callback, post_callback);
	}
	else if (strcmp(key, (*root)->key) == 0) {
		printf("============ WARNING ============\n");
		printf("A Route For \"%s\" Already Exists\n", key);
	}else if (strcmp(key, (*root)->key) > 0) {
		addRoute(&(*root)->right, key, value, user_data, user_data_dealloc, get_callback, post_callback);
	}else {
		addRoute(&(*root)->left, key, value, user_data, user_data_dealloc, get_callback, post_callback);
	}
}

struct Route * search(
		struct Route * root,
		char* key) {
	if (root == NULL) {
		return NULL;
	} 

	if (strcmp(key, root->key) == 0){
		return root;
	}else if (strcmp(key, root->key) > 0) {
		return search(root->right, key);
	}else if (strcmp(key, root->key) < 0) {
		return search(root->left, key);
	}  

	return NULL;
}

