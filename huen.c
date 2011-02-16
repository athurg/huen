#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fetch.h>
#include <gtk/gtk.h>

GtkBuilder *builder;

int parse(char *data, GtkTreeModel *model);
int fetch(const char *url, char *buff);

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "huen.glade", NULL);
	gtk_builder_connect_signals(builder, NULL);

	parse(NULL,GTK_TREE_MODEL(gtk_builder_get_object(builder,"treestore")));

	gtk_main();

	return 0;
}
