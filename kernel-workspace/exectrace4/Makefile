# --- Variablen ---
mod_nam := exectrace1
obj-m   := $(mod_nam).o

KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	 := $(shell pwd)
DEST	:= /lib/modules/$(shell uname -r)/extra
MODCONF := /etc/modules-load.d/$(mod_nam).conf

# --- Targets ---
.PHONY: all clean install uninstall load unload enable disable install-boot install-boot-initramfs debug

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f modules.order Module.symvers

# Installiert das Modul dauerhaft im System (kopiert nach $(DEST) und führt depmod aus)
# Anschließend wird install-boot ausgeführt, initramfs aktualisiert und das Modul sofort geladen.
install: all
	@echo "--- Installing module to $(DEST) ---"
	sudo mkdir -p $(DEST)
	sudo cp -f $(mod_nam).ko $(DEST)/
	sudo depmod -a
	@echo "Module installed to $(DEST). Now enabling boot-load, updating initramfs and loading module..."
	$(MAKE) install-boot
	$(MAKE) install-boot-initramfs
	$(MAKE) load
	@echo "Install complete."

# Entfernt das Modul aus dem System
uninstall: unload
	@echo "--- Removing module from $(DEST) ---"
	sudo rm -f $(DEST)/$(mod_nam).ko
	sudo depmod -a
	@echo "Module removed from $(DEST)."

# Lädt das installierte Modul mittels modprobe
load:
	@echo "--- Loading module $(mod_nam) ---"
	sudo modprobe $(mod_nam) || (sudo dmesg | tail -n 20 && exit 1)

# Entlädt das Modul
unload:
	@echo "--- Unloading module $(mod_nam) ---"
	sudo modprobe -r $(mod_nam) 2>/dev/null || true

# Aktiviert automatisches Laden beim Boot (legt /etc/modules-load.d/exectrace1.conf an)
enable:
	@echo "--- Enabling $(mod_nam) to load at boot ---"
	@echo "$(mod_nam)" | sudo tee $(MODCONF) > /dev/null
	@echo "Created $(MODCONF) (module will be loaded at next boot)."

# Deaktiviert automatisches Laden beim Boot
disable:
	@echo "--- Disabling $(mod_nam) boot load ---"
	sudo rm -f $(MODCONF) || true
	sudo depmod -a
	@echo "Removed $(MODCONF). Module will no longer be auto-loaded at boot."

# Legt modules-load config an (wird von 'install' aufgerufen)
install-boot:
	@echo "--- Creating modules-load config ---"
	@echo "$(mod_nam)" | sudo tee $(MODCONF) > /dev/null
	@echo "Created $(MODCONF). Module will be loaded at boot."

# Aktualisiert initramfs (wird von 'install' aufgerufen). Versucht update-initramfs, fällt zurück auf dracut.
install-boot-initramfs:
	@echo "--- Updating initramfs for all kernels (may take a while) ---"
	@sudo update-initramfs -u -k all 2>/dev/null || \
	( echo "update-initramfs not available or failed, trying dracut..." && sudo dracut -f 2>/dev/null ) || \
	( echo "initramfs update failed or not available on this system; you may need to update it manually." )

# Optional: separate target, falls du initramfs manuell aktualisieren willst
install-boot-initramfs-only:
	@$(MAKE) install-boot-initramfs

debug:
	@echo "--- checking for kprobe header ---"
	@ls $(KDIR)/include/linux/kprobes.h || echo "error: kprobes.h not found"
	@echo "--- checking symbol to hook ---"
	@grep -w bprm_execve /proc/kallsyms || echo "warning: bprm_execve not found"

