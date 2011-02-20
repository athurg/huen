#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <json-glib/json-glib.h>
#include "callback.h" //For GtkTreeModel *model

/*
 * 	JSON解析函数
 *
 * 	从给定的json格式文件中解析影片及其片段信息到GtkTreeModel中
 */
int parse(const char *filename)
{
	JsonParser *parser;
	JsonObject *json_obj;
	JsonArray *urls, *lengths;
	unsigned int blocks;
	GtkTreeIter piter,citer;
	GtkTreeStore *store=GTK_TREE_STORE(model);
	const char *tvname, *url;
	gint64 length;

	parser = json_parser_new();

	if (!json_parser_load_from_file(parser, filename, NULL)) {
		g_object_unref(parser);
		return -1;
	}

	json_obj = json_node_get_object(json_parser_get_root(parser));
	json_obj = json_node_get_object(json_object_get_member(json_obj, "data"));

	//解析并添加影片信息
	tvname = json_object_get_string_member(json_obj,"tvName");
	url    = json_object_get_string_member(json_obj,"coverImg");
	length = json_object_get_int_member(json_obj,"totalBytes");

	gtk_tree_store_append(store, &piter, NULL);
	gtk_tree_store_set(store, &piter, 1,tvname, 2,url, 3,length, -1);

	//解析并添加影片片段信息
	urls = json_object_get_array_member(json_obj,"clipsURL");
	lengths = json_object_get_array_member(json_obj,"clipsBytes");
	blocks = json_array_get_length(urls);

	for (unsigned int i=0; i<blocks; i++) {
		char name[100]={0};
		sprintf(name, "%s_%03d.mp4", tvname, i);
		url = json_array_get_string_element(urls,i);
		length = json_array_get_int_element(lengths,i);

		gtk_tree_store_append(store, &citer, &piter);
		gtk_tree_store_set(store, &citer, 1,name, 2,url, 3,length, -1);
	}

	g_object_unref(parser);
	return 0;
}
