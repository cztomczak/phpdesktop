// Copyright (c) 2018 PHP Desktop, see the Authors file.
// All rights reserved. Licensed under BSD 3-clause license.
// Project website: https://github.com/cztomczak/phpdesktop

#include "gtk.h"
#include "client_handler.h"

#include "include/cef_app.h"
#include "include/base/cef_logging.h"

namespace {
    GtkWidget* g_main_window = NULL;
} // namespace

GtkWidget* create_gtk_window(const char* title, const char* icon,
                             bool center, int width, int height) {
    // Create window.
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_main_window = window;

    // Signals
    g_signal_connect(G_OBJECT(window), "size-allocate",
                     G_CALLBACK(window_size_allocate_signal), NULL);
    g_signal_connect(G_OBJECT(window), "focus-in-event",
                     G_CALLBACK(window_focus_in_signal), NULL);
    g_signal_connect(G_OBJECT(window), "focus-out-event",
                     G_CALLBACK(window_focus_out_signal), NULL);
    g_signal_connect(G_OBJECT(window), "destroy",
                     G_CALLBACK(window_destroy_signal), NULL);
    
    // Default size.
    gtk_window_set_default_size(GTK_WINDOW(window),
            width, height);
    
    // Center.
    if (center) {
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    }
    
    // Title.
    gtk_window_set_title(GTK_WINDOW(window), title);
    
    // CEF requires a container. Embedding browser in a top
    // level window fails.
    #if GTK_CHECK_VERSION(3,0,0)
    GtkWidget* vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    #else
    GtkWidget* vbox = gtk_vbox_new(0, 0);
    #endif
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Show.
    fix_default_x11_visual(GTK_WIDGET(window));
    set_window_icon(GTK_WINDOW(window), icon);
    gtk_widget_show_all(window);

    return vbox;
}

GtkWidget* get_main_window() {
    return g_main_window;
}

::Window get_window_xid(GtkWidget* window) {
    GdkWindow* gdk_window = gtk_widget_get_window(window);
    if (!gdk_window) {
        return 0L;
    }
    #if GTK_CHECK_VERSION(3,0,0)
    return gdk_x11_window_get_xid(gdk_window);
    #else
    return gdk_x11_drawable_get_xid(gdk_window);
    #endif
}

void window_size_allocate_signal(GtkWidget* widget, GtkAllocation *alloc,
                                 gpointer data) {
    ::Window xid = get_window_xid(widget);
    if (!xid) {
        return;
    }
    ::Window xchild = find_child_browser(cef_get_xdisplay(), xid);
    // LOG(INFO) << "window_size_allocate_signal() xchild=" << xchild;
    if (!xchild) {
        return;
    }
    XWindowChanges changes = {0};
    changes.x = alloc->x;
    changes.y = alloc->y;
    changes.width = static_cast<int>(alloc->width);
    changes.height = static_cast<int>(alloc->height);
    XConfigureWindow(cef_get_xdisplay(), xchild,
                     CWX | CWY | CWHeight | CWWidth, &changes);
}

void window_focus_in_signal(GtkWidget* widget, gpointer data) {
    // LOG(INFO) << "window_focus_in_signal";
    ClientHandler *handler = ClientHandler::GetInstance();
    ::Window window_xid = get_window_xid(widget);
    ::Window browser_xid = find_child_browser(cef_get_xdisplay(), window_xid);
    CefRefPtr<CefBrowser> browser = handler->FindBrowserByXid(browser_xid);
    if (browser_xid && browser.get()) {
        // LOG(INFO) << "window_focus_in_signal: Focus browser";
        browser->GetHost()->SetFocus(true);
    }
}

void window_focus_out_signal(GtkWidget* widget, gpointer data) {
    // LOG(INFO) << "window_focus_out_signal";
    ClientHandler *handler = ClientHandler::GetInstance();
    ::Window window_xid = get_window_xid(widget);
    ::Window browser_xid = find_child_browser(cef_get_xdisplay(), window_xid);
    CefRefPtr<CefBrowser> browser = handler->FindBrowserByXid(browser_xid);
    if (browser_xid && browser.get()) {
        // LOG(INFO) << "window_focus_out_signal: Unfocus browser";
        browser->GetHost()->SetFocus(false);
    }
}

void window_destroy_signal(GtkWidget* widget, gpointer data) {
    g_main_window = NULL;
    CefQuitMessageLoop();
}

void set_window_icon(GtkWindow* window, const char* icon) {
    GError *err = NULL;
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(icon, &err);
    if (pixbuf) {
        GdkPixbuf* transparent = gdk_pixbuf_add_alpha(pixbuf, true,
                                                      0xff, 0xff, 0xff);
        if (transparent) {
            GList* icon_list = NULL;
            icon_list = g_list_append(icon_list, transparent);
            LOG(INFO) << "Set app icon: " << icon;
            gtk_window_set_default_icon_list(icon_list);
            gtk_window_set_icon_list(window, icon_list);
            g_list_free(icon_list);
            g_object_unref(transparent);
        }
        g_object_unref(pixbuf);
        if (err) {
            g_error_free(err);
        }
    }
}

::Window find_child_browser(::Display* display, ::Window window) {
    ::Window root;
    ::Window parent;
    ::Window* children;
    ::Window child_window = 0L;
    unsigned int nchildren;
    if (XQueryTree(display, window, &root, &parent, &children, &nchildren)) {
        if (children && nchildren > 1) {
            child_window = children[1]; // sic!
            XFree(children);
        }
    }
    return child_window;
}

void fix_default_x11_visual(GtkWidget* widget) {
    #if GTK_CHECK_VERSION(3,15,1)
    // GTK+ > 3.15.1 uses an X11 visual optimized for GTK+'s OpenGL stuff
    // since revid dae447728d: https://github.com/GNOME/gtk/commit/dae447728d
    // However, it breaks CEF: https://github.com/cztomczak/cefcapi/issues/9
    // Let's use the default X11 visual instead of the GTK's blessed one.
    GdkScreen* screen = gdk_screen_get_default();
    GList* visuals = gdk_screen_list_visuals(screen);
    GdkX11Screen* x11_screen = GDK_X11_SCREEN(screen);
    g_return_if_fail(x11_screen != NULL);
    Visual* default_xvisual = DefaultVisual(GDK_SCREEN_XDISPLAY(x11_screen), GDK_SCREEN_XNUMBER(x11_screen));
    GList* cursor = visuals;
    while (cursor != NULL) {
        GdkVisual* visual = GDK_X11_VISUAL(cursor->data);
        if (default_xvisual->visualid == gdk_x11_visual_get_xvisual(visual)->visualid) {
            gtk_widget_set_visual(widget, visual);
            break; 
        }
        cursor = cursor->next;
    }
    g_list_free(visuals);
    #endif
}