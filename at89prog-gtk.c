/* 
	at89prog
	(c) 2003-2004 Jelmer Vernooij <jelmer@samba.org>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <gtk/gtk.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "pins.h"
#include "at89ser.h"
#include "delays.h"
#include "hexfile.h"

char *buffer = NULL;
char *rcfile;
size_t bufferlen = 0;
GtkWidget* create_aboutwin (void);
GtkWidget* create_settingswin (void);

void update_hex_field() 
{
	/* FIXME */
}

void load_hex_file(const char *file)
{
	FILE *fd = fopen(file, "r");
	if(buffer)g_free(buffer);
	bufferlen = 0; buffer = NULL;
	/* FIXME */
	fclose(fd);
	update_hex_field();
}

void save_hex_file(const char *file)
{
	FILE *fd = fopen(file, "w+");
	/*FIXME*/

	fclose(fd);
}

void on_new_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	if(buffer)g_free(buffer);
	bufferlen = 0; buffer = NULL;
	update_hex_field();
}

void on_open_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *ok_button;
	GtkWidget *cancel_button;
	GtkWidget *openfilewin = gtk_file_selection_new ("Select File");
	guint result;
	gtk_container_set_border_width (GTK_CONTAINER (openfilewin), 10);

	ok_button = GTK_FILE_SELECTION (openfilewin)->ok_button;
	gtk_widget_show (ok_button);
	GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

	cancel_button = GTK_FILE_SELECTION (openfilewin)->cancel_button;
	gtk_widget_show (cancel_button);
	GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

	result = gtk_dialog_run(GTK_DIALOG(openfilewin));
	switch(result) {
	case GTK_RESPONSE_OK:
		load_hex_file(gtk_file_selection_get_filename(GTK_FILE_SELECTION(openfilewin)));
		break;

	default:
		break;
	}
	gtk_widget_destroy(openfilewin);
}


void on_save_as_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *ok_button;
	GtkWidget *cancel_button;
	GtkWidget *savefilewin = gtk_file_selection_new ("Select File");
	guint result;
	gtk_container_set_border_width (GTK_CONTAINER (savefilewin), 10);

	ok_button = GTK_FILE_SELECTION (savefilewin)->ok_button;
	gtk_widget_show (ok_button);
	GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

	cancel_button = GTK_FILE_SELECTION (savefilewin)->cancel_button;
	gtk_widget_show (cancel_button);
	GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

	result = gtk_dialog_run(GTK_DIALOG(savefilewin));
	switch(result) {
	case GTK_RESPONSE_OK:
		save_hex_file(gtk_file_selection_get_filename(GTK_FILE_SELECTION(savefilewin)));
		break;

	default:
		break;
	}
	gtk_widget_destroy(savefilewin);
}


void on_quit_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	gtk_main_quit();
}


void on_settings_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	guint result;
	GtkWidget *settingswin = create_settingswin();
	gtk_widget_show_all(settingswin);
    result = gtk_dialog_run(GTK_DIALOG(settingswin));
	switch(result) {
		case GTK_RESPONSE_OK:
			/*FIXME*/
			break;
		default:
			break;
	}

    gtk_widget_destroy(settingswin);
}


void on_reset_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	deactivate();
}

void on_erase_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	programming();
	erase();
	/* FIXME */
}


void on_download_data_memory_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	/*FIXME*/
}


void on_download_code_memory_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	/*FIXME*/
}


void on_upload_data_memory_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	/*FIXME*/
}


void on_upload_code_memory_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	/*FIXME*/
}


void on_about_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *aboutwin = create_aboutwin();
    gtk_dialog_run(GTK_DIALOG(aboutwin));
    gtk_widget_destroy(aboutwin);
}

void
hex_cell_data_func (GtkTreeViewColumn *col,
                    GtkCellRenderer   *renderer,
                    GtkTreeModel      *model,
                    GtkTreeIter       *iter,
                    gpointer           user_data)
{
	int num = GPOINTER_TO_INT(user_data);
	int hexnum;
	char buf[64];

	gtk_tree_model_get(model, iter, num, &hexnum, -1);

	g_snprintf(buf, sizeof(buf), "%02x", hexnum);

	g_object_set(renderer, "text", buf, NULL);
}


GtkWidget* create_mainwin (void)
{
  GtkWidget *mainwin;
  GtkWidget *vbox1;
  GtkWidget *menubar;
  GtkWidget *menuitem4;
  GtkWidget *menuitem4_menu;
  GtkWidget *new;
  GtkWidget *open;
  GtkWidget *scrolledwindow1;
  GtkWidget *save_as;
  GtkWidget *separatormenuitem1;
  GtkWidget *quit;
  GtkWidget *device;
  GtkWidget *device_menu;
  GtkWidget *settings;
  GtkWidget *reset;
  GtkWidget *erase;
  GtkWidget *lock;
  GtkWidget *download1;
  GtkWidget *download1_menu;
  GtkWidget *download_data_memory;
  GtkWidget *download_code_memory;
  GtkWidget *upload3;
  GtkWidget *upload3_menu;
  GtkWidget *upload_data_memory;
  GtkWidget *upload_code_memory;
  GtkWidget *menuitem7;
  GtkWidget *menuitem7_menu;
  GtkWidget *about;
  GtkWidget *statusbar;
  GtkWidget *hexview;
  GtkCellRenderer *hexrenderer;
  GtkListStore *hexstore;
  GtkAccelGroup *accel_group;
  GtkTreeViewColumn *curcol;

  accel_group = gtk_accel_group_new ();

  mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (mainwin), "AT89Prog");

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (mainwin), vbox1);

  menubar = gtk_menu_bar_new ();
  gtk_box_pack_start (GTK_BOX (vbox1), menubar, FALSE, FALSE, 0);

  menuitem4 = gtk_menu_item_new_with_mnemonic ("_File");
  gtk_container_add (GTK_CONTAINER (menubar), menuitem4);

  menuitem4_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem4), menuitem4_menu);

  new = gtk_image_menu_item_new_from_stock ("gtk-new", accel_group);
  gtk_container_add (GTK_CONTAINER (menuitem4_menu), new);

  open = gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
  gtk_container_add (GTK_CONTAINER (menuitem4_menu), open);

  save_as = gtk_image_menu_item_new_from_stock ("gtk-save-as", accel_group);
  gtk_container_add (GTK_CONTAINER (menuitem4_menu), save_as);

  separatormenuitem1 = gtk_menu_item_new ();
  gtk_container_add (GTK_CONTAINER (menuitem4_menu), separatormenuitem1);
  gtk_widget_set_sensitive (separatormenuitem1, FALSE);

  quit = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
  gtk_container_add (GTK_CONTAINER (menuitem4_menu), quit);

  device = gtk_menu_item_new_with_mnemonic ("_Device");
  gtk_container_add (GTK_CONTAINER (menubar), device);

  device_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (device), device_menu);

  settings = gtk_menu_item_new_with_mnemonic ("_Settings");
  gtk_container_add (GTK_CONTAINER (device_menu), settings);

  separatormenuitem1 = gtk_menu_item_new ();
  gtk_container_add (GTK_CONTAINER (device_menu), separatormenuitem1);
  gtk_widget_set_sensitive (separatormenuitem1, FALSE);

  reset = gtk_menu_item_new_with_mnemonic ("_Reset");
  gtk_container_add (GTK_CONTAINER (device_menu), reset);

  erase = gtk_menu_item_new_with_mnemonic ("_Erase");
  gtk_container_add (GTK_CONTAINER (device_menu), erase);

  lock = gtk_menu_item_new_with_mnemonic ("_Lock");
  gtk_container_add (GTK_CONTAINER (device_menu), lock);

  separatormenuitem1 = gtk_menu_item_new ();
  gtk_container_add (GTK_CONTAINER (device_menu), separatormenuitem1);
  gtk_widget_set_sensitive (separatormenuitem1, FALSE);

  download1 = gtk_menu_item_new_with_mnemonic ("_Download");
  gtk_container_add (GTK_CONTAINER (device_menu), download1);

  download1_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (download1), download1_menu);

  download_data_memory = gtk_menu_item_new_with_mnemonic ("_Data memory");
  gtk_container_add (GTK_CONTAINER (download1_menu), download_data_memory);

  download_code_memory = gtk_menu_item_new_with_mnemonic ("_Code memory");
  gtk_container_add (GTK_CONTAINER (download1_menu), download_code_memory);

  upload3 = gtk_menu_item_new_with_mnemonic ("_Upload");
  gtk_container_add (GTK_CONTAINER (device_menu), upload3);

  upload3_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (upload3), upload3_menu);

  upload_data_memory = gtk_menu_item_new_with_mnemonic ("_Data memory");
  gtk_container_add (GTK_CONTAINER (upload3_menu), upload_data_memory);

  upload_code_memory = gtk_menu_item_new_with_mnemonic ("_Code memory");
  gtk_container_add (GTK_CONTAINER (upload3_menu), upload_code_memory);

  menuitem7 = gtk_menu_item_new_with_mnemonic ("_Help");
  gtk_container_add (GTK_CONTAINER (menubar), menuitem7);

  menuitem7_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem7), menuitem7_menu);

  about = gtk_menu_item_new_with_mnemonic ("_About");
  gtk_container_add (GTK_CONTAINER (menuitem7_menu), about);

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_box_pack_start (GTK_BOX (vbox1), scrolledwindow1, TRUE, TRUE, 0);

  hexview = gtk_tree_view_new();
  hexstore = gtk_list_store_new(17, G_TYPE_LONG, 
					G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR,
					G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR,
					G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR,
					G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR);
  gtk_tree_view_set_model(GTK_TREE_VIEW(hexview), GTK_TREE_MODEL(hexstore));
  curcol = gtk_tree_view_column_new();
  hexrenderer = gtk_cell_renderer_text_new();
  gtk_tree_view_append_column(GTK_TREE_VIEW(hexview), curcol);
  gtk_tree_view_column_pack_start(curcol, hexrenderer, TRUE);
  gtk_tree_view_column_add_attribute(curcol, hexrenderer, "text", 0);
  gtk_tree_view_column_set_cell_data_func(curcol, hexrenderer, hex_cell_data_func, GINT_TO_POINTER(0), NULL);
  {
	  int i;
	  for(i = 1; i <= 16; i++) {
	  	curcol = gtk_tree_view_column_new();
  		hexrenderer = gtk_cell_renderer_text_new();
  		gtk_tree_view_append_column(GTK_TREE_VIEW(hexview), curcol);
		gtk_tree_view_column_pack_start(curcol, hexrenderer, TRUE);
  		gtk_tree_view_column_add_attribute(curcol, hexrenderer, "text", i);
		gtk_tree_view_column_set_cell_data_func(curcol, hexrenderer, hex_cell_data_func, GINT_TO_POINTER(i), NULL);
	  }
  }
  
  statusbar = gtk_statusbar_new ();
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) new, "activate",
                    G_CALLBACK (on_new_activate),
                    NULL);
  g_signal_connect ((gpointer) open, "activate",
                    G_CALLBACK (on_open_activate),
                    NULL);
  g_signal_connect ((gpointer) save_as, "activate",
                    G_CALLBACK (on_save_as_activate),
                    NULL);
  g_signal_connect ((gpointer) quit, "activate",
                    G_CALLBACK (on_quit_activate),
                    NULL);
  g_signal_connect ((gpointer) settings, "activate",
                    G_CALLBACK (on_settings_activate),
                    NULL);
  g_signal_connect ((gpointer) reset, "activate",
                    G_CALLBACK (on_reset_activate),
                    NULL);
  g_signal_connect ((gpointer) erase, "activate",
                    G_CALLBACK (on_erase_activate),
                    NULL);
  g_signal_connect ((gpointer) download_data_memory, "activate",
                    G_CALLBACK (on_download_data_memory_activate),
                    NULL);
  g_signal_connect ((gpointer) download_code_memory, "activate",
                    G_CALLBACK (on_download_code_memory_activate),
                    NULL);
  g_signal_connect ((gpointer) upload_data_memory, "activate",
                    G_CALLBACK (on_upload_data_memory_activate),
                    NULL);
  g_signal_connect ((gpointer) upload_code_memory, "activate",
                    G_CALLBACK (on_upload_code_memory_activate),
                    NULL);
  g_signal_connect ((gpointer) about, "activate",
                    G_CALLBACK (on_about_activate),
                    NULL);

  gtk_window_add_accel_group (GTK_WINDOW (mainwin), accel_group);


  return mainwin;
}

GtkWidget* create_settingswin (void)
{
  GtkWidget *settingswin;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *table1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *settings_combo_port_type;
  GtkWidget *settings_combo_port_type_entry;
  GtkWidget *settings_combo_port_location;
  GtkWidget *settings_combo_port_location_entry;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *table2;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *settings_combo_mosi;
  GtkWidget *settings_combo_mosi_entry;
  GtkWidget *settings_combo_miso;
  GtkWidget *settings_combo_miso_entry;
  GtkWidget *settings_combo_rst;
  GtkWidget *settings_combo_rst_entry;
  GtkWidget *settings_combo_chk;
  GtkWidget *settings_combo_chk_entry;
  GtkWidget *settings_combo_sck;
  GtkWidget *settings_combo_sck_entry;
  GtkWidget *label4;
  GtkWidget *dialog_action_area1;
  GtkWidget *cancelbutton1;
  GtkWidget *applybutton1;
  GtkWidget *okbutton1;

  settingswin = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (settingswin), "Device Settings");

  dialog_vbox1 = GTK_DIALOG (settingswin)->vbox;

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1, TRUE, TRUE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_container_add (GTK_CONTAINER (frame1), table1);

  label2 = gtk_label_new ("Type:");
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  label3 = gtk_label_new ("Location:");
  gtk_table_attach (GTK_TABLE (table1), label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label3), 0, 0.5);

  settings_combo_port_type = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (settings_combo_port_type)->popwin),
                     "GladeParentKey", settings_combo_port_type);
  gtk_table_attach (GTK_TABLE (table1), settings_combo_port_type, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  settings_combo_port_type_entry = GTK_COMBO (settings_combo_port_type)->entry;

  settings_combo_port_location = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (settings_combo_port_location)->popwin),
                     "GladeParentKey", settings_combo_port_location);
  gtk_table_attach (GTK_TABLE (table1), settings_combo_port_location, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  settings_combo_port_location_entry = GTK_COMBO (settings_combo_port_location)->entry;

  label1 = gtk_label_new ("Port");
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label1);

  frame2 = gtk_frame_new (NULL);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, TRUE, TRUE, 0);

  table2 = gtk_table_new (5, 2, FALSE);
  gtk_container_add (GTK_CONTAINER (frame2), table2);

  label5 = gtk_label_new ("MOSI");
  gtk_table_attach (GTK_TABLE (table2), label5, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label5), 0, 0.5);

  label6 = gtk_label_new ("MISO");
  gtk_table_attach (GTK_TABLE (table2), label6, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label6), 0, 0.5);

  label7 = gtk_label_new ("RST");
  gtk_table_attach (GTK_TABLE (table2), label7, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label7), 0, 0.5);

  label8 = gtk_label_new ("CHK");
  gtk_table_attach (GTK_TABLE (table2), label8, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label8), 0, 0.5);

  label9 = gtk_label_new ("SCK");
  gtk_table_attach (GTK_TABLE (table2), label9, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label9), 0, 0.5);

  settings_combo_mosi = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (settings_combo_mosi)->popwin),
                     "GladeParentKey", settings_combo_mosi);
  gtk_table_attach (GTK_TABLE (table2), settings_combo_mosi, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  settings_combo_mosi_entry = GTK_COMBO (settings_combo_mosi)->entry;

  settings_combo_miso = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (settings_combo_miso)->popwin),
                     "GladeParentKey", settings_combo_miso);
  gtk_table_attach (GTK_TABLE (table2), settings_combo_miso, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  settings_combo_miso_entry = GTK_COMBO (settings_combo_miso)->entry;

  settings_combo_rst = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (settings_combo_rst)->popwin),
                     "GladeParentKey", settings_combo_rst);
  gtk_table_attach (GTK_TABLE (table2), settings_combo_rst, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  settings_combo_rst_entry = GTK_COMBO (settings_combo_rst)->entry;

  settings_combo_chk = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (settings_combo_chk)->popwin),
                     "GladeParentKey", settings_combo_chk);
  gtk_table_attach (GTK_TABLE (table2), settings_combo_chk, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  settings_combo_chk_entry = GTK_COMBO (settings_combo_chk)->entry;

  settings_combo_sck = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (settings_combo_sck)->popwin),
                     "GladeParentKey", settings_combo_sck);
  gtk_table_attach (GTK_TABLE (table2), settings_combo_sck, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  settings_combo_sck_entry = GTK_COMBO (settings_combo_sck)->entry;

  label4 = gtk_label_new ("Pins:");
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label4);

  dialog_action_area1 = GTK_DIALOG (settingswin)->action_area;
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_dialog_add_action_widget (GTK_DIALOG (settingswin), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  applybutton1 = gtk_button_new_from_stock ("gtk-apply");
  gtk_dialog_add_action_widget (GTK_DIALOG (settingswin), applybutton1, GTK_RESPONSE_APPLY);
  GTK_WIDGET_SET_FLAGS (applybutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_dialog_add_action_widget (GTK_DIALOG (settingswin), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  return settingswin;
}

GtkWidget* create_aboutwin (void)
{
  GtkWidget *aboutwin;
  GtkWidget *dialog_vbox2;
  GtkWidget *vbox2;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *dialog_action_area2;
  GtkWidget *closebutton1;

  aboutwin = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (aboutwin), "About AT89Prog");
  gtk_window_set_resizable (GTK_WINDOW (aboutwin), FALSE);

  dialog_vbox2 = GTK_DIALOG (aboutwin)->vbox;
  gtk_widget_show (dialog_vbox2);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (dialog_vbox2), vbox2, TRUE, TRUE, 0);

  label10 = gtk_label_new ("<b>AT89Prog</b>");
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (vbox2), label10, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label10), TRUE);

  label11 = gtk_label_new ("(C) 2003-2004 Jelmer Vernooij &lt;jelmer@samba.org&gt;\n\nFor more information, see:\n\nhttp://jelmer.vernstok.nl/oss/at89prog/\n");
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (vbox2), label11, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label11), TRUE);

  dialog_action_area2 = GTK_DIALOG (aboutwin)->action_area;
  gtk_widget_show (dialog_action_area2);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area2), GTK_BUTTONBOX_END);

  closebutton1 = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (closebutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (aboutwin), closebutton1, GTK_RESPONSE_CLOSE);
  GTK_WIDGET_SET_FLAGS (closebutton1, GTK_CAN_DEFAULT);

  return aboutwin;
}

int main (int argc, char *argv[])
{
  GtkWidget *mainwin;

  gtk_init (&argc, &argv);

  rcfile = g_strdup_printf("%s/.at89progrc", g_get_home_dir());
  pins_read_config_file(rcfile);

  mainwin = create_mainwin ();
  gtk_widget_show_all(mainwin);

  gtk_main ();
  deactivate();
  return 0;
}
