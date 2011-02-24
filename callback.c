/**************************************
 *	GtkBuilder界面回调及相关函数
 **************************************/
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "callback.h"
#include "parse.h"
#include "download.h"

//用于自动弹出对话框提示的宏
#define SHOW_MESSAGE(format,args...) \
		({\
			GtkMessageDialog *dlg;\
			dlg = GTK_MESSAGE_DIALOG(gtk_builder_get_object(builder,"msg_dlg"));\
			gtk_message_dialog_format_secondary_markup(dlg,format,## args);\
			gtk_dialog_run(GTK_DIALOG(dlg));\
			gtk_widget_hide(GTK_WIDGET(dlg));\
		})

/*
 * 【列表复选钮】回调函数
 *
 * TODO:
 * 	增加自动选取子项目的功能
 */
void status_toggle_cb(GtkCellRendererToggle *cell_rdr, gchar *path, gpointer user_data)
{
	unsigned int status=0, childs_num;
	GtkTreeIter iter, child_iter;

	gtk_tree_model_get_iter_from_string(model, &iter, path);

	gtk_tree_model_get(model, &iter, 0, &status, -1);
	gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 0,!status, -1);

	//自动更新子成员选中状态
	childs_num = gtk_tree_model_iter_n_children(model, &iter);
	for (unsigned int i=0; i<childs_num; i++) {
		gtk_tree_model_iter_nth_child(model, &child_iter, &iter, i);
		gtk_tree_store_set(GTK_TREE_STORE(model), &child_iter, 0,!status, -1);
	}
}

/**
 *	进度条更新回调函数
 *
 * TODO:
 * 	更新！
 */
int update_progress_bar_cb(void *iter, double dt, double dn, double ut, double un)
{
	unsigned int val;
#if 0
	gdk_threads_enter();

	val =dn*100/dt;
	if (val > 100)
		val = 0;

	gtk_tree_store_set(GTK_TREE_STORE(model), (GtkTreeIter *)iter, 4,val, -1);

	gdk_threads_leave();
#endif
	return 0;
}
/*
 *	执行下载的线程函数
 *
 * 说明：
 * 	本函数用于自动从TreeModel中读取视频信息并下载。
 * 	下载进度将通过update_cb进行反馈。
 */
gpointer do_download(gpointer update_cb)
{
	char *path;
	gint64 blocks;
	char *url, *name;
	unsigned int status=0;
	unsigned int pnum, cnum;//number of parrent and child
	GtkTreeIter iter,child_iter;
	GtkFileChooserButton * path_btn;
	GSList *urls;


	path_btn = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder,"filechooserbutton"));
	path = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(path_btn));

	gtk_tree_model_get_iter_first(model, &iter);
	do{
		//未选中则跳过
		gtk_tree_model_get(model, &iter, 0,&status,-1);
		if (!status)	continue;

		gtk_tree_model_get(model, &iter, 1,&name, -1);
		gtk_tree_model_get(model, &iter, 2,&urls, -1);
		blocks = g_slist_length(urls);
		for (unsigned int i=0; i<blocks; i++) {
			const char *tmpname=NULL, *savename;
			url = (char *)g_slist_nth_data(urls, i);
			tmpname = fetch(url, (curl_progress_callback)update_cb, NULL);
			/*
			 * FIXME
			 * 	这里加入自定义路径，则rename总是返回-1，求真相
			 */
			savename = g_strdup_printf("/tmp/%s-%dof%d.mp4", name, i, blocks);
			if (tmpname) {
				rename(tmpname,savename);
			}
			gtk_tree_store_set(GTK_TREE_STORE(model), &iter, 4,100, -1);
		}
	}while(gtk_tree_model_iter_next(model, &iter));
}

/*
 *	【下载】按钮回调函数
 *
 * TODO:
 * 	增加取消下载的功能
 */
void download_cb(GtkButton *self, gpointer data)
{

	//我们直接创建新线程并调用download_cb执行下载
	if (!g_thread_create(&do_download, update_progress_bar_cb, FALSE, NULL) != 0) {
		SHOW_MESSAGE("无法创建下载线程！");
	} else {
		//gtk_button_set_label(self, "取消");
		;
	}
}


/*
 * 	【添加按钮】回调函数
 *
 * 说明:
 * 	用于获取指定播放页面视频的片段下载地址，并自动添加到treestore中。
 *
 */
void update_list_cb(gpointer url, gpointer user_data)
{
	const char *filename;
	const char *json_url;

	json_url = get_download_url((char *)url);
	filename = fetch(json_url, NULL, NULL);
	if (!filename) {
		SHOW_MESSAGE("获取信息失败\n影片地址：%s", url);
	} else {
		parse(filename);
	}
	unlink(filename);
}

/*
 * 	【搜索按钮】回调函数
 *
 * 说明:
 * 	用于获取指定播放页面视频的片段下载地址，并自动添加到treestore中。
 *
 */
void btn_search_cb(void)
{
	GtkEntry *entry;
	const char *url;
	GSList *list;

	entry = GTK_ENTRY(gtk_builder_get_object(builder,"url_entry"));

	list = get_movie_list(gtk_entry_get_text(entry));
	if (!list){
		SHOW_MESSAGE("没有找到影片！");
	} else {
		g_slist_foreach(list, update_list_cb, NULL);
	}
	g_slist_free(list);
}


/*
 * 	【移除按钮】回调函数
 *
 * 说明:
 * 	用于移除treeview中当前选中项及其子项。
 *
 */
void btn_remove_cb(void)
{
	GtkTreeIter iter;
	GtkTreeView *view;

	view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview"));

	if (gtk_tree_selection_get_selected(gtk_tree_view_get_selection(view), NULL, &iter)){
		gtk_tree_store_remove(GTK_TREE_STORE(model), &iter);
	}

}


