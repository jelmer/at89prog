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
size_t bufferlen = 0;
GtkWidget* create_aboutwin (void);
GtkWidget* create_settingswin (void);

void update_hex_field() 
{
	/* FIXME */
}

void
on_new_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if(buffer)g_free(buffer);
	bufferlen = 0; buffer = NULL;
	update_hex_field();
}

void
on_open_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int firstchar = 0;
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
		/* FIXME */
		
		break;

	default:
		break;
	}
	gtk_widget_destroy(openfilewin);
}


void
on_save_as_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
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
		/* FIXME */
		break;

	default:
		break;
	}
	gtk_widget_destroy(savefilewin);
}


void
on_quit_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();
}


void
on_settings_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
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


void
on_reset_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	deactivate();
}

void
on_erase_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	programming();
	erase();
	/* FIXME */
}


void
on_download_data_memory_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/*FIXME*/
}


void
on_download_code_memory_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/*FIXME*/
}


void
on_upload_data_memory_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/*FIXME*/
}


void
on_upload_code_memory_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/*FIXME*/
}


void
on_about_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *aboutwin = create_aboutwin();
    gtk_dialog_run(GTK_DIALOG(aboutwin));
    gtk_widget_destroy(aboutwin);
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
  GtkWidget *save_as;
  GtkWidget *separatormenuitem1;
  GtkWidget *quit;
  GtkWidget *menuitem5;
  GtkWidget *menuitem5_menu;
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
  GtkListStore *hexstore;
  GtkAccelGroup *accel_group;

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

  menuitem5 = gtk_menu_item_new_with_mnemonic ("_Edit");
  gtk_container_add (GTK_CONTAINER (menubar), menuitem5);

  menuitem5_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem5), menuitem5_menu);

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

  hexview = gtk_tree_view_new();
  hexstore = gtk_list_store_new(17, G_TYPE_LONG, 
					G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR,
					G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR,
					G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR,
					G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR, G_TYPE_UCHAR);
  gtk_tree_view_set_model(GTK_TREE_VIEW(hexview), GTK_TREE_MODEL(hexstore));
  gtk_box_pack_start(GTK_BOX(vbox1), hexview, FALSE, FALSE, 0);
  
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
  GtkWidget *combo_entry1;
  GtkWidget *settings_combo_port_location;
  GtkWidget *combo_entry2;
  GtkWidget *label1;
  GtkWidget *frame2;
  GtkWidget *table2;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *combo3;
  GtkWidget *combo_entry3;
  GtkWidget *combo4;
  GtkWidget *combo_entry4;
  GtkWidget *combo5;
  GtkWidget *combo_entry5;
  GtkWidget *combo6;
  GtkWidget *combo_entry6;
  GtkWidget *combo7;
  GtkWidget *combo_entry7;
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

  combo_entry1 = GTK_COMBO (settings_combo_port_type)->entry;

  settings_combo_port_location = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (settings_combo_port_location)->popwin),
                     "GladeParentKey", settings_combo_port_location);
  gtk_table_attach (GTK_TABLE (table1), settings_combo_port_location, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  combo_entry2 = GTK_COMBO (settings_combo_port_location)->entry;

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

  combo3 = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (combo3)->popwin),
                     "GladeParentKey", combo3);
  gtk_table_attach (GTK_TABLE (table2), combo3, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  combo_entry3 = GTK_COMBO (combo3)->entry;

  combo4 = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (combo4)->popwin),
                     "GladeParentKey", combo4);
  gtk_table_attach (GTK_TABLE (table2), combo4, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  combo_entry4 = GTK_COMBO (combo4)->entry;

  combo5 = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (combo5)->popwin),
                     "GladeParentKey", combo5);
  gtk_table_attach (GTK_TABLE (table2), combo5, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  combo_entry5 = GTK_COMBO (combo5)->entry;

  combo6 = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (combo6)->popwin),
                     "GladeParentKey", combo6);
  gtk_table_attach (GTK_TABLE (table2), combo6, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  combo_entry6 = GTK_COMBO (combo6)->entry;

  combo7 = gtk_combo_new ();
  g_object_set_data (G_OBJECT (GTK_COMBO (combo7)->popwin),
                     "GladeParentKey", combo7);
  gtk_table_attach (GTK_TABLE (table2), combo7, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  combo_entry7 = GTK_COMBO (combo7)->entry;

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

  mainwin = create_mainwin ();
  gtk_widget_show_all(mainwin);

  gtk_main ();
  deactivate();
  return 0;
}
