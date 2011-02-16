#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

int parse(char *buff, GtkTreeModel **model)
{
	int ret;
	JsonParser *parser;
	JsonObject *json_obj;
	JsonArray *urls, *lengths;
	GError *err=NULL;
	unsigned int blocks;
	GtkTreeIter parrent_iter, iter;
	GtkTreeStore *store=GTK_TREE_STORE(model);

	parser = json_parser_new();

	//FIXME:
	//	change this to load_from_buff(buff...)
	ret = json_parser_load_from_file(parser, "dat.txt", &err);
	if (!ret) {
		return -1;
	}

	json_obj = json_node_get_object(json_parser_get_root(parser));
	json_obj = json_node_get_object(json_object_get_member(json_obj, "data"));

	gtk_tree_store_append(store, &parrent_iter, NULL);
	gtk_tree_store_set(store,&parrent_iter,
			0,json_object_get_string_member(json_obj,"tvName"),
			1,json_object_get_string_member(json_obj,"coverImg"),
			2,json_object_get_int_member(json_obj,"totalBytes"),
			-1);
	urls = json_object_get_array_member(json_obj,"clipsURL");
	lengths = json_object_get_array_member(json_obj,"clipsBytes");
	blocks = json_array_get_length(urls);
	for (unsigned int i=0; i<blocks; i++) {
		char name[20]={0};
		sprintf(name, "%d", i);
		gtk_tree_store_append(store, &iter, &parrent_iter);
		gtk_tree_store_set(store, &iter,
				0,name,
				1,json_array_get_string_element(urls,i),
				2,json_array_get_int_element(lengths,i),
				-1);
	}
	return 0;
}
