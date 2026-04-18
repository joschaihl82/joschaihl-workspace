/*
 record_output_strict.c
 Compile:
   gcc $(pkg-config --cflags --libs gstreamer-1.0 glib-2.0) record_output_strict.c -o record_output_strict

 Usage:
   ./record_output_strict            # interaktiv: wählt eine Ausgabesink-Monitor-Quelle
   ./record_output_strict 30         # nimmt 30 Sekunden auf
   ./record_output_strict <monitor_name> 30 # zwingt pulsesrc device string (Monitor-Name) und Dauer

 Behavior:
   - NUR Ausgabegeräte (Audio/Sink) werden berücksichtigt.
   - pulsesrc ohne device wird NICHT verwendet (vermeidet Mikrofone).
   - pipewiresrc wird nur als Output-Fallback verwendet, wenn kein Sink-Monitor gefunden wird.
   - Wenn keine geeignete Output-Quelle gefunden wird, beendet das Programm mit Fehler (keine Aufnahme).
*/

#include <gst/gst.h>
#include <gst/gstdevicemonitor.h>
#include <glib.h>
#if GLIB_CHECK_VERSION(2, 30, 0)
#include <glib-unix.h>
#endif
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

static GstElement *pipeline = NULL;
static GMainLoop *loop = NULL;

/* send EOS in mainloop context so wavenc finalizes header */
static gboolean send_eos_idle(gpointer user_data) {
  (void)user_data;
  if (pipeline) {
    g_print("[INFO] Sende EOS an Pipeline\n");
    gst_element_send_event(pipeline, gst_event_new_eos());
  }
  return G_SOURCE_REMOVE;
}

#if GLIB_CHECK_VERSION(2, 30, 0)
static gboolean on_unix_signal(gpointer user_data) {
  (void)user_data;
  g_idle_add(send_eos_idle, NULL);
  return G_SOURCE_REMOVE;
}
#endif

static volatile sig_atomic_t sigint_received = 0;
static void fallback_sigint_handler(int sig) {
  (void)sig;
  sigint_received = 1;
}
static gboolean poll_sigint_cb(gpointer user_data) {
  (void)user_data;
  if (sigint_received) {
    sigint_received = 0;
    g_idle_add(send_eos_idle, NULL);
    return G_SOURCE_REMOVE;
  }
  return G_SOURCE_CONTINUE;
}

/* filesink handoff callback: reports buffer sizes to confirm data flow */
static void filesink_handoff(GstElement *filesink, GstBuffer *buffer, GstPad *pad, gpointer user_data) {
  (void)filesink; (void)pad; (void)user_data;
  if (!buffer) return;
  gsize size = gst_buffer_get_size(buffer);
  GstClockTime pts = GST_BUFFER_PTS(buffer);
  g_print("[HANDOFF] buffer size=%zu pts=%" GST_TIME_FORMAT "\n", (size_t)size, GST_TIME_ARGS(pts));
}

/* Bus callback: handle EOS, ERROR and element messages (level) */
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
  GMainLoop *loop_local = (GMainLoop*)data;
  switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
      g_print("[BUS] EOS received\n");
      g_main_loop_quit(loop_local);
      break;
    case GST_MESSAGE_ERROR: {
      GError *err = NULL;
      gchar *dbg = NULL;
      gst_message_parse_error(msg, &err, &dbg);
      g_printerr("[BUS] ERROR: %s\n", err ? err->message : "unknown");
      if (dbg) g_printerr("[BUS] DEBUG: %s\n", dbg);
      g_clear_error(&err);
      g_free(dbg);
      g_main_loop_quit(loop_local);
      break;
    }
    case GST_MESSAGE_ELEMENT: {
      GstStructure *s = gst_message_get_structure(msg);
      if (s && gst_structure_has_name(s, "level")) {
        gchar *str = gst_structure_to_string(s);
        g_print("[BUS] level message: %s\n", str);
        g_free(str);
      }
      break;
    }
    default:
      break;
  }
  return G_SOURCE_CONTINUE;
}

/* Build pipeline description; filesink named 'sink' to attach handoff */
static gchar *build_pipeline_desc(const char *source_element, const char *device_string) {
  if (g_strcmp0(source_element, "pipewiresrc") == 0) {
    return g_strdup(
      "pipewiresrc ! audioconvert ! audioresample ! level name=lev interval=100000000 ! "
      "audio/x-raw,format=S16LE,channels=2,rate=44100 ! wavenc ! filesink name=sink location=out.wav");
  }
  /* pulsesrc MUST have device_string here (monitor name) */
  if (device_string && g_strcmp0(source_element, "pulsesrc") == 0) {
    return g_strdup_printf(
      "pulsesrc device=\"%s\" ! audioconvert ! audioresample ! level name=lev interval=100000000 ! "
      "audio/x-raw,format=S16LE,channels=2,rate=44100 ! wavenc ! filesink name=sink location=out.wav",
      device_string);
  }
  return NULL; /* invalid: do not allow pulsesrc without device */
}

/* interactive chooser */
static int choose_index_interactive(char **names, int count) {
  if (count <= 0) return -1;
  if (count == 1) return 0;
  g_print("Gefundene Ausgabesink-Monitore:\n");
  for (int i = 0; i < count; ++i) g_print("  %d) %s\n", i+1, names[i]);
  g_print("Wähle Quelle (1-%d) oder Enter für 1: ", count);
  fflush(stdout);
  char buf[32];
  if (!fgets(buf, sizeof(buf), stdin)) return 0;
  int sel = atoi(buf);
  if (sel < 1 || sel > count) return 0;
  return sel - 1;
}

/* helper: case-insensitive substring */
static gboolean contains_case_insensitive(const char *hay, const char *needle) {
  if (!hay || !needle) return FALSE;
  gchar *low_h = g_utf8_strdown(hay, -1);
  gchar *low_n = g_utf8_strdown(needle, -1);
  gboolean res = (g_strstr_len(low_h, -1, low_n) != NULL);
  g_free(low_h);
  g_free(low_n);
  return res;
}

int main(int argc, char *argv[]) {
  gst_init(&argc, &argv);

  const char *forced_monitor = NULL;
  int duration = 0;
  if (argc >= 2) forced_monitor = argv[1];
  if (argc >= 3) duration = atoi(argv[2]);

  gchar *pipeline_desc = NULL;
  gchar *chosen_monitor = NULL;
  const char *chosen_source = NULL;

  if (forced_monitor) {
    /* user forces a monitor name: treat as pulsesrc device (monitor) */
    chosen_source = "pulsesrc";
    chosen_monitor = g_strdup(forced_monitor);
    pipeline_desc = build_pipeline_desc(chosen_source, chosen_monitor);
    if (!pipeline_desc) {
      g_printerr("[ERROR] Ungültige erzwungene Quelle: %s\n", forced_monitor);
      return 1;
    }
  } else {
    /* enumerate Audio/Sink devices only (output devices) */
    GstDeviceMonitor *monitor = gst_device_monitor_new();
    GstCaps *caps = gst_caps_new_empty_simple("audio/x-raw");
    gst_device_monitor_add_filter(monitor, "Audio/Sink", caps);
    gst_caps_unref(caps);

    if (!gst_device_monitor_start(monitor)) {
      g_printerr("[WARN] DeviceMonitor konnte nicht gestartet werden\n");
    }

    GList *devices = gst_device_monitor_get_devices(monitor);
    char **candidates = NULL;
    char **display_names = NULL;
    int cand_count = 0;

    for (GList *l = devices; l; l = l->next) {
      GstDevice *dev = GST_DEVICE(l->data);
      const gchar *display = gst_device_get_display_name(dev);
      if (!display) display = "(unknown)";

      /* read properties to find monitor/source name */
      GstStructure *props = gst_device_get_properties(dev);
      const gchar *monitor_name = NULL;
      if (props) {
        /* common keys that may contain the monitor/source id */
        monitor_name = gst_structure_get_string(props, "monitor");
        if (!monitor_name) monitor_name = gst_structure_get_string(props, "monitor.source");
        if (!monitor_name) monitor_name = gst_structure_get_string(props, "device");
        if (!monitor_name) monitor_name = gst_structure_get_string(props, "device.string");
        if (!monitor_name) monitor_name = gst_structure_get_string(props, "node.name");
        gst_structure_free(props);
      }

      /* exclude obvious inputs */
      if (contains_case_insensitive(display, "mic") ||
          contains_case_insensitive(display, "micro") ||
          contains_case_insensitive(display, "input") ||
          contains_case_insensitive(display, "capture")) {
        continue;
      }

      /* prefer entries that expose a monitor name or contain 'monitor' in display */
      gboolean looks_like_monitor = FALSE;
      if (monitor_name && contains_case_insensitive(monitor_name, "monitor")) looks_like_monitor = TRUE;
      if (contains_case_insensitive(display, "monitor")) looks_like_monitor = TRUE;

      if (looks_like_monitor && monitor_name) {
        candidates = g_realloc(candidates, sizeof(char*) * (cand_count + 1));
        display_names = g_realloc(display_names, sizeof(char*) * (cand_count + 1));
        candidates[cand_count] = g_strdup(monitor_name);
        display_names[cand_count] = g_strdup(display);
        cand_count++;
      }
    }

    /* cleanup device list */
    for (GList *l = devices; l; l = l->next) gst_object_unref(l->data);
    g_list_free(devices);
    gst_device_monitor_stop(monitor);
    gst_object_unref(monitor);

    if (cand_count > 0) {
      int idx = choose_index_interactive(display_names, cand_count);
      chosen_source = "pulsesrc";
      chosen_monitor = g_strdup(candidates[idx]);
      for (int i = 0; i < cand_count; ++i) {
        g_free(candidates[i]);
        g_free(display_names[i]);
      }
      g_free(candidates);
      g_free(display_names);
      pipeline_desc = build_pipeline_desc(chosen_source, chosen_monitor);
    } else {
      /* No sink-monitor found: allow pipewiresrc only as explicit output fallback */
      if (gst_element_factory_find("pipewiresrc")) {
        chosen_source = "pipewiresrc";
        pipeline_desc = build_pipeline_desc(chosen_source, NULL);
        g_print("[INFO] Kein Sink-Monitor gefunden; verwende pipewiresrc als Output-Fallback\n");
      } else {
        g_printerr("[ERROR] Keine Ausgabesink-Monitor-Quellen gefunden und pipewiresrc nicht verfügbar.\n");
        g_printerr("[ERROR] Das Programm verweigert die Verwendung von Eingangsquellen (Mikrofon).\n");
        return 2;
      }
    }
  }

  g_print("[INFO] Verwendete Pipeline: %s\n", pipeline_desc ? pipeline_desc : "(null)");
  pipeline = gst_parse_launch(pipeline_desc, NULL);
  g_free(pipeline_desc);

  if (!pipeline) {
    g_printerr("[ERROR] Pipeline konnte nicht erstellt werden\n");
    if (chosen_monitor) g_free(chosen_monitor);
    return 1;
  }

  /* attach handoff to filesink named 'sink' to confirm buffers arrive */
  GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
  if (sink) {
    g_object_set(sink, "signal-handoffs", TRUE, NULL);
    g_signal_connect(sink, "handoff", G_CALLBACK(filesink_handoff), NULL);
    gst_object_unref(sink);
  } else {
    g_printerr("[WARN] filesink 'sink' nicht gefunden; Handoff nicht möglich\n");
  }

  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  /* main loop + bus watch */
  loop = g_main_loop_new(NULL, FALSE);
  GstBus *bus = gst_element_get_bus(pipeline);
  gst_bus_add_watch(bus, (GstBusFunc)bus_call, loop);
  gst_object_unref(bus);

#if GLIB_CHECK_VERSION(2, 30, 0)
  g_unix_signal_add(SIGINT, on_unix_signal, NULL);
#else
  signal(SIGINT, fallback_sigint_handler);
  g_timeout_add(200, poll_sigint_cb, NULL);
#endif

  if (duration > 0) {
    g_print("[INFO] Aufnahme für %d Sekunden...\n", duration);
    g_timeout_add_seconds(duration, (GSourceFunc)send_eos_idle, NULL);
  } else {
    g_print("[INFO] Aufnahme läuft (Ctrl+C zum Beenden)...\n");
  }

  g_main_loop_run(loop);

  /* cleanup and report file size */
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  struct stat st;
  if (stat("out.wav", &st) == 0) {
    g_print("[INFO] out.wav Größe: %lld bytes\n", (long long)st.st_size);
    if (st.st_size == 0) {
      g_printerr("[WARN] out.wav ist 0 Bytes groß — keine Daten aufgenommen.\n");
      g_printerr("[TIP] Prüfe mit 'pactl list short sources' ob Monitor-Quellen existieren oder teste:\n");
      g_printerr("  gst-launch-1.0 pipewiresrc ! audioconvert ! wavenc ! filesink location=test.wav\n");
    }
  } else {
    g_printerr("[ERROR] Konnte out.wav nicht statten\n");
  }

  if (chosen_monitor) g_free(chosen_monitor);
  g_main_loop_unref(loop);
  return 0;
}
