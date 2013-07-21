#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <curl/curl.h>
#include "callback.h"

GtkBuilder *builder;
GtkTreeModel *model;

int main(int argc, char **argv)
{
	curl_global_init(CURL_GLOBAL_ALL);

	g_thread_init(NULL);
	gdk_threads_enter();

	gtk_init(&argc, &argv);


	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "huen.glade", NULL);
	gtk_builder_connect_signals(builder, NULL);

	model = GTK_TREE_MODEL(gtk_builder_get_object(builder, "treestore"));

	gtk_main();
	gdk_threads_leave();

	return 0;
}
