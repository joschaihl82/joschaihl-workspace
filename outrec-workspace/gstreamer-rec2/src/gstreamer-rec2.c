#include <gst/gst.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline;
    GstBus *bus;
    GstMessage *msg;

    gst_init(&argc, &argv);

    // Zeitstempel erzeugen
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char filename[256];
    strftime(filename, sizeof(filename), "%Y-%m-%d-%H-%M-%S.wav", t);

    printf("Aufnahme (Audioausgang) -> %s\n", filename);

    // 👉 WICHTIG: Monitor-Device verwenden!
    const char *device = "auto_null.monitor"; // Fallback / wird oft überschrieben

    // Pipeline zusammenbauen
    char pipeline_desc[1024];
    snprintf(pipeline_desc, sizeof(pipeline_desc),
        "pulsesrc device=%s ! audioconvert ! audioresample ! wavenc ! filesink location=%s",
        device, filename);

    GError *error = NULL;
    pipeline = gst_parse_launch(pipeline_desc, &error);

    if (!pipeline) {
        fprintf(stderr, "Pipeline-Fehler: %s\n", error->message);
        g_error_free(error);
        return -1;
    }

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    printf("Aufnahme läuft (Systemsound). Stop mit Ctrl+C.\n");

    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                     GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
            gst_message_parse_error(msg, &err, &debug_info);
            fprintf(stderr, "Fehler: %s\n", err->message);
            g_error_free(err);
            g_free(debug_info);
        }

        gst_message_unref(msg);
    }

    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
