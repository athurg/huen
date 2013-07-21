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
	JsonArray *urlarray;
	unsigned int blocks;
	GtkTreeIter iter;
	const char *tvname;
	gint64 length;
	GSList *urls;

	parser = json_parser_new();

	if (!json_parser_load_from_file(parser, filename, NULL)) {
		g_object_unref(parser);
		return -1;
	}

	json_obj = json_node_get_object(json_parser_get_root(parser));
	json_obj = json_node_get_object(json_object_get_member(json_obj, "data"));

	//解析并添加影片信息
	tvname = json_object_get_string_member(json_obj,"tvName");
	length = json_object_get_int_member(json_obj,"totalBytes");

	//解析并添加影片片段信息
	urlarray = json_object_get_array_member(json_obj,"clipsURL");
	blocks = json_array_get_length(urlarray);

	for (unsigned int i=0; i<blocks; i++) {
		const char *url;
		url = json_array_get_string_element(urlarray,i);
		urls = g_slist_append(urls, strdup(url));
	}

	gtk_tree_store_append(GTK_TREE_STORE(model), &iter, NULL);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter,
			0, 1,		//selected
			1, tvname,	//name
			2, urls,	//url slist
			3, length,	//size
			4, 0,		//download process
			-1);

	g_object_unref(parser);
	return 0;
}
