#include<gtk/gtk.h>

GtkWidget *window;
GtkWidget *entry;
GtkEntryBuffer *entry_buffer;
int *arr;
int size = 0;
int Current_Index =0;
int Switch_Index = 0;
#define InsertionSpeed 150000
gboolean ProgramEnd=FALSE;

gboolean quit_main_loop(gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;
    g_main_loop_quit(loop);
    return G_SOURCE_REMOVE;
}

void close_app(GtkButton *button, GtkEntry *entry) {
    ProgramEnd=TRUE;
}


void array_number_append(int value){

    arr = realloc(arr,(size + 1) * sizeof(*arr));

    arr[size] = value;
    size++;
}


int array_value_max_get(void){
    int value_max = 0;
    int i;

    for (i = 0; i < size; i++){
        if (arr[i] > value_max) value_max = arr[i];
    }

    return value_max; 
}


int buffer_str_to_number(const char *buffer){
    int value = 0;
    int sign = 1;
    int i;

    for (i = 0; buffer[i] != 0; i++){
      if(buffer[i]!=' '){
         value = value * 10;
         if(buffer[i]=='-'){
            sign=-1;
            i++;
         }
      if(buffer[i]>=0) value =value + buffer[i] - 0x30;
           else value =(value + buffer[i] - 0x30)*(-1);
      }
    }

    return value;
}


//anis
void insertion_sort_area_draw_func(GtkDrawingArea *drawing_area,cairo_t *cr,int width,int height,gpointer data){
    
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_set_line_width(cr, 2.0);
    cairo_rectangle(cr, 5, 5, width - 10, height - 10);
    cairo_stroke(cr);

    width -= 10;
    height -= 5;

    double array_rectangle_width = (double)width / size;
    int value_max = array_value_max_get() * 1.45; /* for vertical scaling  - 1.25 for padding on the top */

    cairo_select_font_face(cr,"Sans",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, array_rectangle_width - 2);
    cairo_set_source_rgb(cr, 0, 0, 0);

    char array_value_str[11];

    int i;
    for (i = 0; i < size; i++)
    {
        if (i == Current_Index)
            cairo_set_source_rgba(cr, 0, 0, 255, 1);
        else if (i == Switch_Index - 1)
            cairo_set_source_rgba(cr, 150, 0, 0, 1);
        else
            cairo_set_source_rgba(cr, 0, 0, 0, 1);


        cairo_rectangle(cr,5 + i * array_rectangle_width,height - (height - 5) * (double)(arr[i] / (double)value_max),array_rectangle_width,(height - 5) * (double)(arr[i] / (double)value_max));
        sprintf(array_value_str, "%d", arr[i]);
        cairo_save(cr);
        cairo_move_to(cr,5 + i * array_rectangle_width + array_rectangle_width - 2,height - ((height - 5) * (double)(arr[i] / (double)value_max)) - 5);
        cairo_rotate(cr, -M_PI / 2.0);
        cairo_show_text(cr, array_value_str);
        cairo_restore(cr);

        cairo_stroke(cr);
    }
}


//anis
GSourceFunc drawing_area_redraw_callback(gpointer user_data){
    gtk_widget_queue_draw(GTK_WIDGET(user_data));

    return G_SOURCE_REMOVE;
}


void *insertion_sort(void *data){
    int tmp;

    for (Current_Index = 1; Current_Index < size; Current_Index++)
    {
        for (Switch_Index = Current_Index;
             Switch_Index > 0 && arr[Switch_Index - 1] > arr[Switch_Index];
             Switch_Index--)
        {
            tmp = arr[Switch_Index];
            arr[Switch_Index] = arr[Switch_Index - 1];
            arr[Switch_Index - 1] = tmp;

            g_idle_add(G_SOURCE_FUNC(drawing_area_redraw_callback), data);
            g_usleep(InsertionSpeed);
        }
    }

    Current_Index = 0;
    Switch_Index = 0;
    gtk_widget_queue_draw(GTK_WIDGET(data));

    return NULL;
}


//anis
void insertion_sort_async(gpointer data){
    GThread *thread = g_thread_new("thread", insertion_sort, data);
    g_thread_unref(thread);
}


void sort_clicked(GtkWidget *widget, gpointer data){
    insertion_sort_async(data);
}


void insert_clicked(GtkWidget *widget, gpointer data){
    const char *entry_buffer_str = gtk_entry_buffer_get_text(entry_buffer);
    int value = buffer_str_to_number(entry_buffer_str);

    array_number_append(value);

    gtk_entry_buffer_set_text(entry_buffer, "", 0);

    gtk_widget_queue_draw(data);
    gtk_window_set_focus((GtkWindow *)window, entry);  
}


void delete_clicked(GtkWidget *widget, gpointer data){
    int i;

    if (Current_Index == 0){
        const char *entry_buffer_str = gtk_entry_buffer_get_text(entry_buffer);
        int value = buffer_str_to_number(entry_buffer_str);

        for (i = 0; i < size; i++){
            if (value == arr[i]) break;
        }

        if (i >= size) return;
    }
    else i = Current_Index;

    for (; i < size - 1; i++){
        arr[i] = arr[i + 1];
    }

    Current_Index = -1;
    size--;

    Current_Index = 0;
    Switch_Index = 0;

    gtk_entry_buffer_set_text(entry_buffer, "", 0);

    gtk_widget_queue_draw(data);
    gtk_window_set_focus((GtkWindow *)window, entry);
}


void search_clicked(GtkWidget *widget, gpointer data){
    const char *entry_buffer_str = gtk_entry_buffer_get_text(entry_buffer);
    int value = buffer_str_to_number(entry_buffer_str);
    int i;

    for (i = 0; i < size; i++){
        if (value == arr[i]) Current_Index = i;
    }

    gtk_entry_buffer_set_text(entry_buffer, "", 0);

    gtk_widget_queue_draw(data);
    gtk_window_set_focus((GtkWindow *)window, entry);
}

GMainLoop *main(){
    gtk_init();

    window = gtk_window_new();
    gtk_window_set_default_size((GtkWindow *)window, 800, 500);
    gtk_window_set_title((GtkWindow *)window, "insertion sort");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child((GtkWindow *)window, vbox);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func((GtkDrawingArea *)drawing_area,insertion_sort_area_draw_func,NULL,NULL);
    gtk_widget_set_hexpand(drawing_area, TRUE);
    gtk_widget_set_vexpand(drawing_area, TRUE);
    gtk_box_append((GtkBox *)vbox, drawing_area);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append((GtkBox *)vbox, hbox);

    GtkWidget *button = gtk_button_new_with_label("SORT");
    gtk_widget_set_hexpand(button, TRUE);
    g_signal_connect(button, "clicked", G_CALLBACK(sort_clicked), drawing_area);
    gtk_box_append((GtkBox *)hbox, button);

    button = gtk_button_new_with_label("INSERT");
    gtk_widget_set_hexpand(button, TRUE);
    g_signal_connect(button, "clicked", G_CALLBACK(insert_clicked), drawing_area);
    gtk_box_append((GtkBox *)hbox, button);

    button = gtk_button_new_with_label("DELETE");
    gtk_widget_set_hexpand(button, TRUE);
    g_signal_connect(button, "clicked", G_CALLBACK(delete_clicked), drawing_area);
    gtk_box_append((GtkBox *)hbox, button);

    button = gtk_button_new_with_label("SEARCH");
    gtk_widget_set_hexpand(button, TRUE);
    g_signal_connect(button, "clicked", G_CALLBACK(search_clicked), drawing_area);
    gtk_box_append((GtkBox *)hbox, button);

    entry_buffer = gtk_entry_buffer_new("", 0);
    entry = gtk_entry_new_with_buffer(entry_buffer);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_box_append((GtkBox *)hbox, entry);

    gtk_window_present((GtkWindow *)window);
    gtk_window_set_focus((GtkWindow *)window, entry);

    GMainLoop *loop = g_main_loop_new(NULL, TRUE);

    g_signal_connect(window,"destroy",G_CALLBACK(close_app),NULL);

    while (!ProgramEnd) {
        g_idle_add(quit_main_loop, loop);
        g_main_context_iteration(NULL,FALSE);
    }

    return loop;
}