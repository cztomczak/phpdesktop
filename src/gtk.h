// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

GtkWidget* create_gtk_window(const char* title, const char* icon,
                             bool center, int width, int height);
::Window get_window_xid(GtkWidget* window);
GtkWidget* get_main_window();
void window_size_allocate_signal(GtkWidget* widget, GtkAllocation *alloc,
                                 gpointer data);
void window_focus_in_signal(GtkWidget* widget, gpointer data);
void window_focus_out_signal(GtkWidget* widget, gpointer data);
void window_destroy_signal(GtkWidget* widget, gpointer data);
void set_window_icon(GtkWindow* window, const char* icon);
::Window find_child_browser(::Window window);
void fix_default_x11_visual(GtkWidget* widget);
