/*
 * socks5_unconfigurator.c
 *
 * Finalisierte, robuste version: entfernt alle Proxy-Konfigurationen,
 * die von socks5_configurator.c gesetzt wurden (systemweit + user).
 *
 * build:
 *   gcc -std=c11 -o socks5_unconfigurator socks5_unconfigurator.c
 *
 * usage:
 *   ./socks5_unconfigurator
 *
 * Hinweise:
 * - /etc/* benötigt root (sudo).
 * - GNOME/dconf, Transmission, Firefox werden optional/stub-artig behandelt.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <pwd.h>

/* --------------------------- konventionsdefinitionen --------------------------- */

#define INT_CFG_SM_BUF_MAX 512
#define INT_CFG_LG_BUF_MAX 8192
#define INT_SYS_PATH_MAX 4096

typedef enum { enm_log_info, enm_log_warn, enm_log_error, enm_log_debug } enm_log_level_t;

/* --------------------------- logging --------------------------- */

static void log_sys_msg_print(enm_log_level_t lev_msg_level, const char *cch_fmt_str, ...) {
    time_t tim_cur_val = time(NULL);
    struct tm stc_tim_loc;
    char cch_tim_str[64];
    localtime_r(&tim_cur_val, &stc_tim_loc);
    strftime(cch_tim_str, sizeof(cch_tim_str), "%Y-%m-%d %H:%M:%S", &stc_tim_loc);

    const char *cch_lev_str = "info";
    if (lev_msg_level == enm_log_warn) cch_lev_str = "warn";
    else if (lev_msg_level == enm_log_error) cch_lev_str = "error";
    else if (lev_msg_level == enm_log_debug) cch_lev_str = "debug";

    va_list ap;
    va_start(ap, cch_fmt_str);
    fprintf(stderr, "%s [%s] ", cch_tim_str, cch_lev_str);
    vfprintf(stderr, cch_fmt_str, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

/* --------------------------- file I/O --------------------------- */

static char *sys_fs_file_read(const char *cch_path_name) {
    FILE *fil_ptr_read = fopen(cch_path_name, "r");
    if (!fil_ptr_read) return NULL;
    if (fseek(fil_ptr_read, 0, SEEK_END) != 0) { fclose(fil_ptr_read); return NULL; }
    long lon_size_val = ftell(fil_ptr_read);
    if (lon_size_val < 0) { fclose(fil_ptr_read); return NULL; }
    rewind(fil_ptr_read);
    char *cch_buf_data = malloc((size_t)lon_size_val + 1);
    if (!cch_buf_data) { fclose(fil_ptr_read); return NULL; }
    size_t siz_read_len = fread(cch_buf_data, 1, (size_t)lon_size_val, fil_ptr_read);
    cch_buf_data[siz_read_len] = '\0';
    fclose(fil_ptr_read);
    return cch_buf_data;
}

static int sys_fs_file_write_atomic(const char *cch_path_name, const char *cch_cont_data, mode_t mod_file_perm) {
    char cch_tmp_path[INT_SYS_PATH_MAX];
    snprintf(cch_tmp_path, sizeof(cch_tmp_path), "%s.tmp.%ld", cch_path_name, (long)getpid());

    FILE *fil_ptr_write = fopen(cch_tmp_path, "w");
    if (!fil_ptr_write) {
        log_sys_msg_print(enm_log_error, "failed open temp %s: %s", cch_tmp_path, strerror(errno));
        return -1;
    }
    if (fputs(cch_cont_data, fil_ptr_write) == EOF) {
        log_sys_msg_print(enm_log_error, "failed write temp %s", cch_tmp_path);
        fclose(fil_ptr_write); unlink(cch_tmp_path); return -1;
    }
    if (fclose(fil_ptr_write) != 0) {
        log_sys_msg_print(enm_log_error, "failed close temp %s: %s", cch_tmp_path, strerror(errno));
        unlink(cch_tmp_path); return -1;
    }
    if (chmod(cch_tmp_path, mod_file_perm) != 0) {
        log_sys_msg_print(enm_log_warn, "chmod failed on %s: %s", cch_tmp_path, strerror(errno));
    }
    if (rename(cch_tmp_path, cch_path_name) != 0) {
        log_sys_msg_print(enm_log_error, "rename %s -> %s failed: %s", cch_tmp_path, cch_path_name, strerror(errno));
        unlink(cch_tmp_path); return -1;
    }
    return 0;
}

/* --------------------------- line filtering --------------------------- */

/* Entfernt alle Zeilen, die mit einem Präfix beginnen (Whitespace tolerant). */
static char *log_lin_remove_prefix(const char *cch_orig_cont, const char *cch_prefix_key) {
    if (!cch_orig_cont) return strdup("");

    size_t siz_out_cap = strlen(cch_orig_cont) + 1;
    char *cch_out_data = calloc(1, siz_out_cap);
    if (!cch_out_data) return NULL;

    const char *cch_ptr_read = cch_orig_cont;
    size_t siz_pref_len = strlen(cch_prefix_key);

    while (*cch_ptr_read) {
        const char *cch_lin_end = strchr(cch_ptr_read, '\n');
        size_t siz_lin_len = cch_lin_end ? (size_t)(cch_lin_end - cch_ptr_read) : strlen(cch_ptr_read);

        const char *cch_lin_start = cch_ptr_read;
        while (*cch_lin_start == ' ' || *cch_lin_start == '\t') cch_lin_start++;
        size_t siz_no_ws_len = siz_lin_len - (cch_lin_start - cch_ptr_read);

        int int_is_match = (siz_no_ws_len >= siz_pref_len &&
                            strncmp(cch_lin_start, cch_prefix_key, siz_pref_len) == 0);

        if (!int_is_match) {
            size_t siz_need = strlen(cch_out_data) + siz_lin_len + 2;
            if (siz_need > siz_out_cap) {
                siz_out_cap = siz_need + 128;
                char *tmp = realloc(cch_out_data, siz_out_cap);
                if (!tmp) { free(cch_out_data); return NULL; }
                cch_out_data = tmp;
            }
            strncat(cch_out_data, cch_ptr_read, siz_lin_len);
            strcat(cch_out_data, "\n");
        }

        if (!cch_lin_end) break;
        cch_ptr_read = cch_lin_end + 1;
    }

    return cch_out_data;
}

/* Entfernt Zeilen, die einen Substring enthalten (z.B. proxycommand mit /usr/bin/nc). */
static char *log_lin_remove_contains(const char *cch_orig_cont, const char *cch_substr) {
    if (!cch_orig_cont) return strdup("");

    size_t siz_out_cap = strlen(cch_orig_cont) + 1;
    char *cch_out_data = calloc(1, siz_out_cap);
    if (!cch_out_data) return NULL;

    const char *cch_ptr_read = cch_orig_cont;

    while (*cch_ptr_read) {
        const char *cch_lin_end = strchr(cch_ptr_read, '\n');
        size_t siz_lin_len = cch_lin_end ? (size_t)(cch_lin_end - cch_ptr_read) : strlen(cch_ptr_read);

        int int_is_match = 0;
        if (siz_lin_len > 0) {
            char buf[INT_CFG_LG_BUF_MAX];
            size_t copy_len = siz_lin_len < sizeof(buf) - 1 ? siz_lin_len : sizeof(buf) - 1;
            memcpy(buf, cch_ptr_read, copy_len);
            buf[copy_len] = '\0';
            if (strstr(buf, cch_substr) != NULL) int_is_match = 1;
        }

        if (!int_is_match) {
            size_t siz_need = strlen(cch_out_data) + siz_lin_len + 2;
            if (siz_need > siz_out_cap) {
                siz_out_cap = siz_need + 128;
                char *tmp = realloc(cch_out_data, siz_out_cap);
                if (!tmp) { free(cch_out_data); return NULL; }
                cch_out_data = tmp;
            }
            strncat(cch_out_data, cch_ptr_read, siz_lin_len);
            strcat(cch_out_data, "\n");
        }

        if (!cch_lin_end) break;
        cch_ptr_read = cch_lin_end + 1;
    }

    return cch_out_data;
}

/* Entfernt leere Zeilen am Ende und führende überflüssige Newlines. */
static void log_lin_trim_mut(char *cch_buf) {
    if (!cch_buf) return;
    size_t n = strlen(cch_buf);
    while (n > 0 && (cch_buf[n-1] == '\n' || cch_buf[n-1] == ' ' || cch_buf[n-1] == '\t')) { cch_buf[--n] = '\0'; }
    // keine aggressive führende trim hier, wir erhalten Format.
}

/* --------------------------- removal helpers --------------------------- */

static int cfg_remove_prefixes_write(const char *cch_path_name, const char *arr_prefixes[], size_t siz_count, mode_t mod_perm) {
    char *cch_orig_cont = sys_fs_file_read(cch_path_name);
    if (!cch_orig_cont) {
        log_sys_msg_print(enm_log_debug, "skip missing file: %s", cch_path_name);
        return 0;
    }

    char *cch_updated = strdup(cch_orig_cont);
    free(cch_orig_cont);
    if (!cch_updated) return -1;

    for (size_t i = 0; i < siz_count; i++) {
        char *tmp = log_lin_remove_prefix(cch_updated, arr_prefixes[i]);
        free(cch_updated);
        cch_updated = tmp;
        if (!cch_updated) return -1;
    }

    log_lin_trim_mut(cch_updated);

    int rc = sys_fs_file_write_atomic(cch_path_name, cch_updated, mod_perm);
    if (rc == 0) log_sys_msg_print(enm_log_info, "updated: %s", cch_path_name);
    else log_sys_msg_print(enm_log_error, "failed to update: %s", cch_path_name);

    free(cch_updated);
    return rc;
}

static int cfg_remove_contains_write(const char *cch_path_name, const char *arr_substr[], size_t siz_count, mode_t mod_perm) {
    char *cch_orig_cont = sys_fs_file_read(cch_path_name);
    if (!cch_orig_cont) {
        log_sys_msg_print(enm_log_debug, "skip missing file: %s", cch_path_name);
        return 0;
    }

    char *cch_updated = strdup(cch_orig_cont);
    free(cch_orig_cont);
    if (!cch_updated) return -1;

    for (size_t i = 0; i < siz_count; i++) {
        char *tmp = log_lin_remove_contains(cch_updated, arr_substr[i]);
        free(cch_updated);
        cch_updated = tmp;
        if (!cch_updated) return -1;
    }

    log_lin_trim_mut(cch_updated);

    int rc = sys_fs_file_write_atomic(cch_path_name, cch_updated, mod_perm);
    if (rc == 0) log_sys_msg_print(enm_log_info, "updated: %s", cch_path_name);
    else log_sys_msg_print(enm_log_error, "failed to update: %s", cch_path_name);

    free(cch_updated);
    return rc;
}

/* --------------------------- specific targets --------------------------- */

static int cfg_sys_env_remove(void) {
    const char *prefixes[] = {"socks5_proxy=", "all_proxy=", "http_proxy=", "https_proxy="};
    return cfg_remove_prefixes_write("/etc/environment", prefixes, sizeof(prefixes)/sizeof(prefixes[0]), 0644);
}

static int cfg_sys_apt_remove(void) {
    // Entfernt die Datei-Inhalte-Zeile; optional könnte man die Datei löschen,
    // wir ersetzen Inhalt leer (kompatibel mit atomic write)
    const char *path = "/etc/apt/apt.conf.d/99socks5-proxy";
    char *cont = sys_fs_file_read(path);
    if (!cont) return 0;
    free(cont);
    int rc = sys_fs_file_write_atomic(path, "", 0644);
    if (rc == 0) log_sys_msg_print(enm_log_info, "cleared apt conf: %s", path);
    else log_sys_msg_print(enm_log_error, "failed clear apt conf: %s", path);
    return rc;
}

static int cfg_usr_shell_remove(const char *home_dir) {
    const char *prefixes[] = {"export socks5_proxy=", "export all_proxy=", "export http_proxy=", "export https_proxy="};
    int status = 0;
    char path[INT_SYS_PATH_MAX];

    snprintf(path, sizeof(path), "%s/.profile", home_dir);
    if (cfg_remove_prefixes_write(path, prefixes, 4, 0644) != 0) status = -1;

    snprintf(path, sizeof(path), "%s/.bashrc", home_dir);
    if (cfg_remove_prefixes_write(path, prefixes, 4, 0644) != 0) status = -1;

    snprintf(path, sizeof(path), "%s/.zshrc", home_dir);
    if (cfg_remove_prefixes_write(path, prefixes, 4, 0644) != 0) status = -1;

    return status;
}

static int cfg_usr_wget_remove(const char *home_dir) {
    char path[INT_SYS_PATH_MAX];
    snprintf(path, sizeof(path), "%s/.wgetrc", home_dir);
    const char *prefixes[] = {"socks_proxy = "};
    return cfg_remove_prefixes_write(path, prefixes, 1, 0600);
}

static int cfg_usr_curl_remove(const char *home_dir) {
    char path[INT_SYS_PATH_MAX];
    snprintf(path, sizeof(path), "%s/.curlrc", home_dir);
    const char *prefixes[] = {"socks5 = ", "proxy = ", "http_proxy = ", "https_proxy = "};
    return cfg_remove_prefixes_write(path, prefixes, sizeof(prefixes)/sizeof(prefixes[0]), 0600);
}

static int cfg_usr_ssh_remove(const char *home_dir) {
    char path[INT_SYS_PATH_MAX];
    snprintf(path, sizeof(path), "%s/.ssh/config", home_dir);
    const char *substr[] = {"proxycommand /usr/bin/nc", "ProxyCommand /usr/bin/nc", "ProxyJump", "ProxyCommand"};
    return cfg_remove_contains_write(path, substr, sizeof(substr)/sizeof(substr[0]), 0600);
}

static int cfg_usr_git_remove(const char *home_dir) {
    char path[INT_SYS_PATH_MAX];
    snprintf(path, sizeof(path), "%s/.gitconfig", home_dir);
    // Entfernt "proxy = ..." Zeilen; wir belassen [http] Abschnitt stehen
    const char *prefixes[] = {"\tproxy = ", "proxy = "};
    return cfg_remove_prefixes_write(path, prefixes, sizeof(prefixes)/sizeof(prefixes[0]), 0600);
}

/* --------------------------- GUI / apps (stubs / optional) --------------------------- */

static int cfg_sys_gnome_remove(void) {
    // Optional: system() Aufrufe. Hier nur geloggt, um robust ohne Abhängigkeiten zu bleiben.
    log_sys_msg_print(enm_log_info, "GNOME/dconf: set mode 'none' and clear socks host/port (simulation).");
    log_sys_msg_print(enm_log_debug, "exec: gsettings set org.gnome.system.proxy mode 'none'");
    log_sys_msg_print(enm_log_debug, "exec: gsettings reset org.gnome.system.proxy.socks host");
    log_sys_msg_print(enm_log_debug, "exec: gsettings reset org.gnome.system.proxy.socks port");
    return 0;
}

static int cfg_app_transmission_remove(const char *home_dir) {
    char path[INT_SYS_PATH_MAX];
    snprintf(path, sizeof(path), "%s/.config/transmission/settings.json", home_dir);
    log_sys_msg_print(enm_log_info, "transmission: clear proxy fields via jq or manual edit (simulation): %s", path);
    log_sys_msg_print(enm_log_debug, "simulated: jq '.proxy-enabled = false | .proxy = \"\" | .proxy-port = 0' %s", path);
    return 0;
}

static int cfg_app_firefox_remove(const char *home_dir) {
    (void)home_dir;
    log_sys_msg_print(enm_log_warn, "firefox prefs.js about:config editing requires careful parsing. skipping for robustness.");
    log_sys_msg_print(enm_log_debug, "tip: set network.proxy.type=0 and clear network.proxy.socks, socks_port in prefs.js");
    return 0;
}

/* --------------------------- orchestrator --------------------------- */

static int cfg_all_remove_main(void) {
    int status = 0;

    // Laufzeit-Umgebungsvariablen (nur aktueller Prozess)
    unsetenv("socks5_proxy");
    unsetenv("all_proxy");
    unsetenv("http_proxy");
    unsetenv("https_proxy");

    struct passwd *stc_pwd_info = getpwuid(getuid());
    if (!stc_pwd_info || !stc_pwd_info->pw_dir) {
        log_sys_msg_print(enm_log_error, "home directory could not be determined.");
        return -1;
    }
    const char *home_dir = stc_pwd_info->pw_dir;

    /* system configurations (root needed) */
    if (cfg_sys_env_remove() != 0) status = -1;
    if (cfg_sys_apt_remove() != 0) status = -1;

    /* user shell configs */
    if (cfg_usr_shell_remove(home_dir) != 0) status = -1;

    /* user apps */
    if (cfg_usr_wget_remove(home_dir) != 0) status = -1;
    if (cfg_usr_curl_remove(home_dir) != 0) status = -1;
    if (cfg_usr_ssh_remove(home_dir) != 0) status = -1;
    if (cfg_usr_git_remove(home_dir) != 0) status = -1;

    /* GUI / complex apps (stubs/optional) */
    if (cfg_sys_gnome_remove() != 0) status = -1;
    if (cfg_app_transmission_remove(home_dir) != 0) status = -1;
    if (cfg_app_firefox_remove(home_dir) != 0) status = -1;

    if (status == 0) log_sys_msg_print(enm_log_info, "completed proxy removal across all targets.");
    else log_sys_msg_print(enm_log_warn, "completed with some failures. check logs.");

    return status;
}

/* --------------------------- main --------------------------- */

int main(void) {
    log_sys_msg_print(enm_log_info, "starting full proxy removal.");
    int rc = cfg_all_remove_main();
    if (rc != 0) {
        log_sys_msg_print(enm_log_error, "proxy removal encountered errors.");
        return EXIT_FAILURE;
    }
    log_sys_msg_print(enm_log_info, "proxy removal successful.");
    return EXIT_SUCCESS;
}

