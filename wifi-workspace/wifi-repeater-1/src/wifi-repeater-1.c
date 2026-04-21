/*
 * wifi_repeater_fixed_full.c
 *
 * Vollständig korrigierte Version des Repeater-Orchestrators.
 *
 * - erkennt korrekt das Upstream managed Interface (ignoriert AP_IF)
 * - normalisiert phy (z.B. phy0)
 * - ermittelt Upstream-Kanal und setzt hostapd channel entsprechend
 * - erkennt DFS-Bereich und warnt
 * - versucht mehrere iw-Varianten zum Anlegen einer VIF
 * - prüft, ob Interface bereits existiert
 * - startet hostapd/dnsmasq, überwacht hostapd-Log auf Fehler
 * - bei Fehlern: sammelt dmesg und ethtool, versucht AP-only Fallback
 * - räumt bei SIGINT/SIGTERM auf
 *
 * Achtung: Proof-of-Concept. Nutze mit Vorsicht.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define AP_IF "wlan0_ap"
#define AP_SSID "MyRepeater"
#define AP_PSK  "ChangeMe123"
#define AP_IP   "192.168.50.1/24"
#define DHCP_RANGE_START "192.168.50.10"
#define DHCP_RANGE_END   "192.168.50.100"

static char upstream_if[64] = {0};
static char phy_norm[64] = {0};
static int did_create_vif = 0;
static int did_start_services = 0;

static void trim(char *s) {
    if (!s) return;
    char *p = s;
    while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == ' ' || s[len-1] == '\t' || s[len-1] == '\r' || s[len-1] == '\n')) {
        s[len-1] = '\0';
        len--;
    }
}

static int run_cmd_capture(const char *cmd, char *out, size_t outlen) {
    FILE *fp = popen(cmd, "r");
    if (!fp) return -1;
    size_t pos = 0;
    while (fgets(out + pos, outlen - pos, fp)) {
        pos = strlen(out);
        if (pos >= outlen - 1) break;
    }
    int rc = pclose(fp);
    return rc;
}

static int try_cmd(const char *cmd) {
    int rc = system(cmd);
    if (rc == -1) return -1;
    if (WIFEXITED(rc)) return WEXITSTATUS(rc);
    return rc;
}

static void cleanup(void) {
    char cmd[256];
    fprintf(stderr, "Aufräumen: Dienste stoppen, Schnittstelle entfernen, iptables bereinigen...\n");
    system("pkill hostapd 2>/dev/null || true");
    system("pkill dnsmasq 2>/dev/null || true");
    snprintf(cmd, sizeof(cmd), "ip link del %s 2>/dev/null || true", AP_IF);
    system(cmd);
    if (upstream_if[0]) {
        snprintf(cmd, sizeof(cmd), "iptables -t nat -D POSTROUTING -o %s -j MASQUERADE 2>/dev/null || true", upstream_if);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "iptables -D FORWARD -i %s -o %s -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || true", upstream_if, AP_IF);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "iptables -D FORWARD -i %s -o %s -j ACCEPT 2>/dev/null || true", AP_IF, upstream_if);
        system(cmd);
    }
}

static void sig_handler(int sig) {
    (void)sig;
    fprintf(stderr, "\nSignal empfangen: Aufräumen und beenden...\n");
    cleanup();
    exit(0);
}

/* Prüft, ob Interface existiert */
static int iface_exists(const char *ifname) {
    char cmd[128], out[256] = {0};
    snprintf(cmd, sizeof(cmd), "ip link show %s 2>/dev/null || true", ifname);
    run_cmd_capture(cmd, out, sizeof(out));
    return (strstr(out, ifname) != NULL);
}

/* Ermittelt erstes managed Interface, ignoriert AP_IF */
static int detect_upstream_interface(char *buf, size_t buflen) {
    char out[8192] = {0};
    if (run_cmd_capture("iw dev 2>/dev/null", out, sizeof(out)) != 0) return 0;
    char *p = out;
    char last_iface[64] = {0};
    while (*p) {
        char line[256] = {0};
        char *nl = strchr(p, '\n');
        size_t l = nl ? (size_t)(nl - p) : strlen(p);
        if (l >= sizeof(line)) l = sizeof(line)-1;
        strncpy(line, p, l);
        line[l] = '\0';
        trim(line);
        if (strncmp(line, "Interface ", 10) == 0) {
            strncpy(last_iface, line + 10, sizeof(last_iface)-1);
            trim(last_iface);
        } else if (strcmp(line, "type managed") == 0) {
            if (last_iface[0] && strcmp(last_iface, AP_IF) != 0) {
                strncpy(buf, last_iface, buflen-1);
                return 1;
            }
        }
        if (!nl) break;
        p = nl + 1;
    }
    return 0;
}

/* Ermittelt phy token wie phy0 */
static int detect_phy(char *outbuf, size_t outlen) {
    char out[65536] = {0};
    if (run_cmd_capture("iw list 2>/dev/null", out, sizeof(out)) != 0) {
        if (run_cmd_capture("iw phy 2>/dev/null", out, sizeof(out)) != 0) return 0;
    }
    char *p = strstr(out, "Wiphy");
    if (p) {
        char *phy = strstr(p, "phy");
        if (phy) {
            int i = 0;
            while (phy[i] && phy[i] != ' ' && phy[i] != '\n' && phy[i] != ':' && i < (int)outlen-1) {
                outbuf[i] = phy[i];
                i++;
            }
            outbuf[i] = '\0';
            return 1;
        }
    }
    p = strstr(out, "phy#");
    if (p) {
        char tmp[64] = {0};
        int i = 0, j = 0;
        while (p[i] && p[i] != '\n' && j < (int)sizeof(tmp)-1) {
            if (p[i] == '#') { i++; continue; }
            tmp[j++] = p[i++];
        }
        tmp[j] = '\0';
        strncpy(outbuf, tmp, outlen-1);
        return 1;
    }
    p = strstr(out, "phy");
    if (p) {
        char tmp[64] = {0};
        int i = 0;
        while (p[i] && p[i] != ' ' && p[i] != '\n' && i < (int)sizeof(tmp)-1) {
            tmp[i] = p[i];
            i++;
        }
        tmp[i] = '\0';
        strncpy(outbuf, tmp, outlen-1);
        return 1;
    }
    return 0;
}

/* Ermittelt Kanal der Upstream-Schnittstelle */
static int get_upstream_channel(const char *iface) {
    if (!iface || !iface[0]) return 0;
    char cmd[128], out[1024] = {0};
    snprintf(cmd, sizeof(cmd), "iw dev %s info 2>/dev/null", iface);
    if (run_cmd_capture(cmd, out, sizeof(out)) != 0) return 0;
    char *p = strstr(out, "channel ");
    if (!p) return 0;
    p += strlen("channel ");
    int ch = atoi(p);
    return ch;
}

static int is_dfs_channel(int ch) {
    return (ch >= 52 && ch <= 140);
}

/* hostapd/dnsmasq conf schreiben */
static int write_hostapd_conf(const char *ap_if, int channel) {
    FILE *f = fopen("/tmp/hostapd_repeater.conf", "w");
    if (!f) return 0;
    fprintf(f,
        "interface=%s\n"
        "driver=nl80211\n"
        "ssid=%s\n"
        "hw_mode=g\n"
        "channel=%d\n"
        "wpa=2\n"
        "wpa_passphrase=%s\n"
        "wpa_key_mgmt=WPA-PSK\n"
        "rsn_pairwise=CCMP\n"
        "country_code=DE\n"
        "ieee80211d=1\n"
        "ieee80211h=1\n",
        ap_if, AP_SSID, channel, AP_PSK);
    fclose(f);
    return 1;
}

static int write_dnsmasq_conf(const char *ap_if) {
    FILE *d = fopen("/tmp/dnsmasq_repeater.conf", "w");
    if (!d) return 0;
    fprintf(d, "interface=%s\n", ap_if);
    fprintf(d, "dhcp-range=%s,%s,12h\n", DHCP_RANGE_START, DHCP_RANGE_END);
    fprintf(d, "bind-interfaces\n");
    fclose(d);
    return 1;
}

static void start_services(void) {
    char cmd[512];
    system("pkill dnsmasq 2>/dev/null || true");
    snprintf(cmd, sizeof(cmd), "dnsmasq --conf-file=/tmp/dnsmasq_repeater.conf > /tmp/dnsmasq_repeater.log 2>&1 &");
    system(cmd);
    system("pkill hostapd 2>/dev/null || true");
    snprintf(cmd, sizeof(cmd), "hostapd -dd /tmp/hostapd_repeater.conf > /tmp/hostapd_repeater.log 2>&1 &");
    system(cmd);
    did_start_services = 1;
}

static int hostapd_log_has_failure(void) {
    char out[16384] = {0};
    run_cmd_capture("tail -n 200 /tmp/hostapd_repeater.log 2>/dev/null", out, sizeof(out));
    if (strstr(out, "Failed to set beacon parameters") ||
        strstr(out, "Interface initialization failed") ||
        strstr(out, "Unable to setup interface") ||
        strstr(out, "AP-DISABLED")) {
        return 1;
    }
    return 0;
}

/* Versucht VIF mit mehreren Varianten anzulegen */
static int create_vif(const char *phy, const char *up_if, const char *ap_if) {
    if (iface_exists(ap_if)) {
        fprintf(stderr, "Schnittstelle %s existiert bereits, benutze sie.\n", ap_if);
        return 1;
    }
    char cmd[256];
    const char *attempts[] = {
        "iw phy %s interface add %s type __ap 2>/dev/null",
        "iw phy %s interface add %s type ap 2>/dev/null",
        "iw dev %s interface add %s type __ap 2>/dev/null",
        "iw dev %s interface add %s type ap 2>/dev/null"
    };
    for (size_t i = 0; i < sizeof(attempts)/sizeof(attempts[0]); ++i) {
        if (i < 2) snprintf(cmd, sizeof(cmd), attempts[i], phy, ap_if);
        else snprintf(cmd, sizeof(cmd), attempts[i], up_if, ap_if);
        printf("Versuche: %s\n", cmd);
        int rc = try_cmd(cmd);
        if (rc == 0) {
            did_create_vif = 1;
            return 1;
        }
    }
    return 0;
}

/* Trenne upstream Interface (nmcli oder iw) */
static void disconnect_upstream(const char *iface) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "nmcli device disconnect %s 2>/dev/null || true", iface);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "iw dev %s disconnect 2>/dev/null || true", iface);
    system(cmd);
}

int main(void) {
    if (geteuid() != 0) {
        fprintf(stderr, "Dieses Programm muss als root ausgefuehrt werden.\n");
        return 1;
    }
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    /* Upstream Interface erkennen */
    if (!detect_upstream_interface(upstream_if, sizeof(upstream_if))) {
        fprintf(stderr, "Konnte kein 'managed' Interface erkennen. Bitte verbinde zuerst mit einem WLAN oder gib das Interface manuell an.\n");
        return 2;
    }
    printf("Erkanntes Upstream-Interface: %s\n", upstream_if);

    /* phy erkennen */
    if (!detect_phy(phy_norm, sizeof(phy_norm))) {
        fprintf(stderr, "Konnte phy nicht ermitteln (iw list/iw phy Ausgabe). Stelle sicher, dass 'iw' installiert ist.\n");
        return 3;
    }
    trim(phy_norm);
    printf("Erkannter phy: %s\n", phy_norm);

    /* iw list Ausschnitt */
    char iwlist[65536] = {0};
    run_cmd_capture("iw list 2>/dev/null", iwlist, sizeof(iwlist));
    printf("---- 'iw list' (erste 2000 Zeichen) ----\n");
    for (size_t i = 0; i < 2000 && i < strlen(iwlist); ++i) putchar(iwlist[i]);
    printf("\n---- Ende Ausschnitt ----\n\n");

    /* Upstream Kanal ermitteln */
    int ch = get_upstream_channel(upstream_if);
    if (ch == 0) {
        fprintf(stderr, "Warnung: Konnte Upstream-Kanal nicht ermitteln; hostapd wird mit Standard-Kanal 6 starten.\n");
        ch = 6;
    } else {
        printf("Upstream-Kanal: %d\n", ch);
        if (is_dfs_channel(ch)) {
            fprintf(stderr, "Warnung: Upstream-Interface %s ist auf einem 5GHz/DFS-Kanal (%d).\n", upstream_if, ch);
            fprintf(stderr, "DFS kann STA+AP parallel verhindern. Wenn VIF-Erstellung fehlschlaegt, versuche 2.4GHz oder zweiten Adapter.\n");
        }
    }

    /* VIF anlegen */
    if (!create_vif(phy_norm, upstream_if, AP_IF)) {
        fprintf(stderr, "Fehler: Konnte virtuelle Schnittstelle '%s' nicht anlegen.\n", AP_IF);
        char dmesg_out[32768] = {0};
        run_cmd_capture("dmesg | tail -n 80 2>/dev/null", dmesg_out, sizeof(dmesg_out));
        printf("---- dmesg (letzte 80 Zeilen) ----\n%s\n---- Ende dmesg ----\n", dmesg_out);
        char ethtool_out[2048] = {0};
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "ethtool -i %s 2>/dev/null || true", upstream_if);
        run_cmd_capture(cmd, ethtool_out, sizeof(ethtool_out));
        printf("---- Treiberinfo (ethtool -i %s) ----\n%s\n---- Ende Treiberinfo ----\n", upstream_if, ethtool_out);
        cleanup();
        return 4;
    }
    printf("Virtuelle Schnittstelle %s angelegt.\n", AP_IF);

    /* Interface hoch und IP setzen */
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ip link set %s up", AP_IF);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "ip addr add %s dev %s 2>/dev/null || true", AP_IP, AP_IF);
    system(cmd);

    /* Forwarding und NAT */
    system("sysctl -w net.ipv4.ip_forward=1 >/dev/null 2>&1");
    snprintf(cmd, sizeof(cmd), "iptables -t nat -A POSTROUTING -o %s -j MASQUERADE", upstream_if);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "iptables -A FORWARD -i %s -o %s -m state --state RELATED,ESTABLISHED -j ACCEPT", upstream_if, AP_IF);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "iptables -A FORWARD -i %s -o %s -j ACCEPT", AP_IF, upstream_if);
    system(cmd);

    /* Konfigurationen schreiben */
    if (!write_hostapd_conf(AP_IF, ch)) {
        fprintf(stderr, "Konnte hostapd.conf nicht schreiben.\n");
        cleanup();
        return 5;
    }
    if (!write_dnsmasq_conf(AP_IF)) {
        fprintf(stderr, "Konnte dnsmasq.conf nicht schreiben.\n");
        cleanup();
        return 6;
    }

    /* Dienste starten */
    start_services();
    sleep(2);

    if (hostapd_log_has_failure()) {
        fprintf(stderr, "hostapd meldet Fehler beim Initialisieren der AP-Schnittstelle. Sammle Logs und versuche AP-only Fallback.\n");
        char hostlog[16384] = {0};
        run_cmd_capture("tail -n 200 /tmp/hostapd_repeater.log 2>/dev/null", hostlog, sizeof(hostlog));
        printf("---- hostapd log (tail) ----\n%s\n---- Ende hostapd log ----\n", hostlog);
        char dmesg_out[32768] = {0};
        run_cmd_capture("dmesg | tail -n 120 2>/dev/null", dmesg_out, sizeof(dmesg_out));
        printf("---- dmesg (letzte 120 Zeilen) ----\n%s\n---- Ende dmesg ----\n", dmesg_out);
        char ethtool_out[2048] = {0};
        snprintf(cmd, sizeof(cmd), "ethtool -i %s 2>/dev/null || true", upstream_if);
        run_cmd_capture(cmd, ethtool_out, sizeof(ethtool_out));
        printf("---- Treiberinfo (ethtool -i %s) ----\n%s\n---- Ende Treiberinfo ----\n", upstream_if, ethtool_out);

        /* AP-only Fallback */
        fprintf(stderr, "Versuche AP-only Fallback: trenne STA (%s) und starte hostapd erneut.\n", upstream_if);
        disconnect_upstream(upstream_if);
        sleep(1);
        system("pkill hostapd 2>/dev/null || true");
        snprintf(cmd, sizeof(cmd), "hostapd -dd /tmp/hostapd_repeater.conf > /tmp/hostapd_repeater.log 2>&1 &");
        system(cmd);
        sleep(2);
        if (hostapd_log_has_failure()) {
            fprintf(stderr, "AP-only Versuch ebenfalls fehlgeschlagen. Empfohlen: zweiten WLAN-Adapter verwenden oder hostapd Debug-Ausgabe analysieren.\n");
            cleanup();
            return 7;
        } else {
            fprintf(stderr, "AP-only Versuch erfolgreich. AP läuft jetzt ohne Upstream-STA.\n");
            printf("hostapd und dnsmasq laufen. AP '%s' (%s) sollte verfuegbar sein.\n", AP_SSID, AP_IP);
            printf("Zum Beenden STRG+C drücken (räumt auf).\n");
            while (1) sleep(60);
        }
    } else {
        printf("hostapd initialisiert erfolgreich. AP '%s' (%s) sollte verfuegbar sein.\n", AP_SSID, AP_IP);
        printf("Zum Beenden STRG+C drücken (räumt auf).\n");
        while (1) sleep(60);
    }

    return 0;
}
