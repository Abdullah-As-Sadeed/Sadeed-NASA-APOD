/* By Abdullah As-Sadeed */

/*
gcc ./Sadeed_NASA_APOD.cpp -o ./Sadeed_NASA_APOD `pkg-config --cflags --libs gtk+-3.0` -lcurl -ljson-c
*/

#include "csignal"
#include "curl/curl.h"
#include "iostream"
#include "gtk/gtk.h"
#include "json-c/json.h"
#include "stdio.h"
#include "string.h"

#define TERMINAL_ANSI_COLOR_RED "\x1b[31m"
#define TERMINAL_ANSI_COLOR_RESET "\x1b[0m"

#define API_KEY "0fcOCWDdMYKiIx3EwU4ntNbmrjuOFmSUgjmotmoy"

#define IMAGE_FILE "Latest_APOD_Image.jpg"

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 720

struct Memory_Buffer
{
    char *memory;
    size_t size;
};

size_t Write_Callback(void *pointer, size_t size, size_t member_count, void *user_data)
{
    size_t total_size = size * member_count;
    struct Memory_Buffer *buffer = (struct Memory_Buffer *)user_data;

    buffer->memory = (char *)realloc(buffer->memory, buffer->size + total_size + 1);
    if (buffer->memory == nullptr)
    {
        printf(TERMINAL_ANSI_COLOR_RED "Failed to reallocate memory.\n" TERMINAL_ANSI_COLOR_RESET);
        return 0;
    }

    memcpy(&(buffer->memory[buffer->size]), pointer, total_size);
    buffer->size += total_size;
    buffer->memory[buffer->size] = '\0';

    return total_size;
}

void Download_Image(const char *URL)
{
    CURL *curl_image = curl_easy_init();
    if (curl_image)
    {
        FILE *file_pointer = fopen(IMAGE_FILE, "wb");
        if (file_pointer)
        {
            curl_easy_setopt(curl_image, CURLOPT_URL, URL);
            curl_easy_setopt(curl_image, CURLOPT_WRITEFUNCTION, nullptr);
            curl_easy_setopt(curl_image, CURLOPT_WRITEDATA, file_pointer);
            curl_easy_perform(curl_image);

            curl_easy_cleanup(curl_image);
            fclose(file_pointer);
        }
        else
        {
            printf(TERMINAL_ANSI_COLOR_RED "Failed to open image file for writing.\n" TERMINAL_ANSI_COLOR_RESET);
        }
    }
    else
    {
        printf(TERMINAL_ANSI_COLOR_RED "Failed to initialize cURL to download image.\n" TERMINAL_ANSI_COLOR_RESET);
    }
}

gboolean Open_URL_Externally(GtkWidget *widget, gchar *URL)
{
    gtk_show_uri_on_window(nullptr, URL, GDK_CURRENT_TIME, nullptr);

    g_free(URL);
    return true;
}

void Handle_Signal(int signal)
{
    if (signal == SIGINT)
    {
        printf(TERMINAL_ANSI_COLOR_RED "\n\nYou interrupted me by SIGINT signal.\n" TERMINAL_ANSI_COLOR_RESET);
        exit(signal);
    }
}

int main(int argument_count, char *argument_values[])
{
    signal(SIGINT, Handle_Signal);

    gtk_init(&argument_count, &argument_values);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), "Sadeed NASA APOD : NASA Astronomy Picture of the Day");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(window), scrolled_window);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(scrolled_window), box);

    GtkWidget *image = gtk_image_new();
    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);

    GtkWidget *title_label = gtk_label_new(nullptr);
    gtk_box_pack_start(GTK_BOX(box), title_label, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(title_label), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL(title_label), TRUE);
    gtk_widget_set_margin_top(title_label, 10);

    GtkWidget *date_label = gtk_label_new(nullptr);
    gtk_box_pack_start(GTK_BOX(box), date_label, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(date_label), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL(date_label), TRUE);

    GtkWidget *explanation_label = gtk_label_new(nullptr);
    gtk_box_pack_start(GTK_BOX(box), explanation_label, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(explanation_label), GTK_JUSTIFY_FILL);
    gtk_label_set_line_wrap(GTK_LABEL(explanation_label), TRUE);
    gtk_widget_set_margin_start(explanation_label, 20);
    gtk_widget_set_margin_end(explanation_label, 20);

    GtkWidget *copyright_label = gtk_label_new(nullptr);
    gtk_box_pack_start(GTK_BOX(box), copyright_label, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(copyright_label), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL(copyright_label), TRUE);
    gtk_widget_set_margin_bottom(copyright_label, 10);

    GtkWidget *button = gtk_button_new_with_label("Open HD Image");
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);

    struct Memory_Buffer buffer = {nullptr, 0};

    CURL *curl_JSON = curl_easy_init();
    if (curl_JSON)
    {
        curl_easy_setopt(curl_JSON, CURLOPT_URL, "https://api.nasa.gov/planetary/apod?api_key=" API_KEY);
        curl_easy_setopt(curl_JSON, CURLOPT_WRITEFUNCTION, Write_Callback);
        curl_easy_setopt(curl_JSON, CURLOPT_WRITEDATA, &buffer);
        curl_easy_perform(curl_JSON);

        curl_easy_cleanup(curl_JSON);
    }
    else
    {
        printf(TERMINAL_ANSI_COLOR_RED "Failed to initialize cURL to download JSON.\n" TERMINAL_ANSI_COLOR_RESET);
    }

    if (buffer.memory)
    {
        struct json_object *JSON_object = json_tokener_parse(buffer.memory), *JSON_image_URL, *JSON_title, *JSON_date, *JSON_explanation, *JSON_copyright, *JSON_HD_image_URL;
        json_object_object_get_ex(JSON_object, "url", &JSON_image_URL);
        json_object_object_get_ex(JSON_object, "title", &JSON_title);
        json_object_object_get_ex(JSON_object, "date", &JSON_date);
        json_object_object_get_ex(JSON_object, "explanation", &JSON_explanation);
        json_object_object_get_ex(JSON_object, "copyright", &JSON_copyright);
        json_object_object_get_ex(JSON_object, "hdurl", &JSON_HD_image_URL);

        gchar *title = g_strdup(json_object_get_string(JSON_title));
        gchar *date = g_strdup(json_object_get_string(JSON_date));
        gchar *explanation = g_strdup(json_object_get_string(JSON_explanation));
        gchar *copyright = g_strdup(json_object_get_string(JSON_copyright));
        gchar *HD_image_URL = g_strdup(json_object_get_string(JSON_HD_image_URL));

        const char *image_url = json_object_get_string(JSON_image_URL);
        Download_Image(image_url);
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(IMAGE_FILE, nullptr);
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);

        gchar *title_markup = g_markup_printf_escaped("<b>%s</b>", title);
        gtk_label_set_markup(GTK_LABEL(title_label), title_markup);

        gchar *date_markup = g_markup_printf_escaped("<small>%s</small>", date);
        gtk_label_set_markup(GTK_LABEL(date_label), date_markup);

        gtk_label_set_text(GTK_LABEL(explanation_label), explanation);

        g_strdelimit(copyright, "\n", ' ');
        gchar *copyright_markup = g_markup_printf_escaped("<small>%s</small>", copyright);
        gtk_label_set_markup(GTK_LABEL(copyright_label), copyright_markup);

        g_signal_connect(button, "clicked", G_CALLBACK(Open_URL_Externally), HD_image_URL);

        json_object_put(JSON_object);
        free(buffer.memory);
        g_free(title);
        g_free(date);
        g_free(explanation);
        g_free(copyright);
        g_free(HD_image_URL);
        g_free(title_markup);
        g_free(date_markup);
        g_free(copyright_markup);
    }

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
