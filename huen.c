#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <curl/curl.h>

GtkBuilder *builder;
gpointer do_download(gpointer data);

int parse(char *data, GtkTreeModel *model);
int fetch(const char *url, char *buff, curl_progress_callback cb, void *user_data);


/*
 * 是否下载复选钮回调函数
 *
 * TODO:
 * 	增加自动选取子项目的功能
 */
void toggle_status_cb(GtkCellRendererToggle *cell_rdr, gchar *path, gpointer user_data)
{
	int status=0;
	GtkTreeIter iter;
	GtkTreeModel *model;


	model = GTK_TREE_MODEL(gtk_builder_get_object(builder,"treestore"));
	gtk_tree_model_get_iter_from_string(model, &iter, path);

	gtk_tree_model_get(model, &iter, 0, &status, -1);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0,!status, -1);
}

/*
 *	【开始下载】按钮回调函数
 */
int download_cb(void)
{
	GtkTreeModel *model;

	model = GTK_TREE_MODEL(gtk_builder_get_object(builder,"treestore"));
	if (!g_thread_create(&do_download, model, FALSE, NULL) != 0) {
		g_warning("创建下载线程失败！");
	}
}

gpointer do_download(gpointer data)
{
	GtkTreeModel *model;
	GtkTreeIter iter,child_iter;
	char *url, *name;
	gint64 blocks;
	unsigned int pnum, cnum;//number of parrent and child
	unsigned int i=0,j=0;


	model = (GtkTreeModel *) data;
	gtk_tree_model_get_iter_first(model, &iter);
	do{
		blocks = gtk_tree_model_iter_n_children(model, &iter);
		for (j=0; j<blocks; j++) {
			gtk_tree_model_iter_nth_child(model, &child_iter, &iter, j);
			gtk_tree_model_get(model, &child_iter, 1,&name,-1);
			gtk_tree_model_get(model, &child_iter, 2,&url,-1);

			//TODO:
			//	添加动态更新进度条的功能
			fetch(url, name, NULL, &child_iter);
			g_message("新建下载：%s ==> %s", url, name);
			gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter, 4, 100.0, -1);
		}
		gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 4, 100.0, -1);
	}while(gtk_tree_model_iter_next(model, &iter));
}

int main(int argc, char **argv)
{
	curl_global_init(CURL_GLOBAL_ALL);

	g_thread_init(NULL);
	gtk_init(&argc, &argv);


	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, "huen.glade", NULL);
	gtk_builder_connect_signals(builder, NULL);

	parse("dat.txt",GTK_TREE_MODEL(gtk_builder_get_object(builder,"treestore")));
	parse("dat2.txt",GTK_TREE_MODEL(gtk_builder_get_object(builder,"treestore")));

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	return 0;
}
