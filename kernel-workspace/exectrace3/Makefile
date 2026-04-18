# --- Variablen ---
mod_nam := exectrace1
obj-m   := $(mod_nam).o

KDIR    := /lib/modules/$(shell uname -r)/build
PWD     := $(shell pwd)
DEST    := /lib/modules/$(shell uname -r)/extra

# --- Targets ---
.PHONY: all clean install uninstall load unload debug

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f modules.order Module.symvers

# Installiert das Modul dauerhaft im System
install: all
	@echo "--- Installing module to $(DEST) ---"
	sudo mkdir -p $(DEST)
	sudo cp $(mod_nam).ko $(DEST)
	sudo depmod -a
	@echo "Module installed. Use 'make load' to activate it via modprobe."

# Entfernt das Modul aus dem System
uninstall: unload
	@echo "--- Removing module from $(DEST) ---"
	sudo rm -f $(DEST)/$(mod_nam).ko
	sudo depmod -a

# Lädt das installierte Modul mittels modprobe
load:
	@echo "--- Loading module $(mod_nam) ---"
	sudo modprobe $(mod_nam) || (sudo dmesg | tail -n 20 && exit 1)

# Entlädt das Modul
unload:
	@echo "--- Unloading module $(mod_nam) ---"
	sudo modprobe -r $(mod_nam) 2>/dev/null || true

debug:
	@echo "--- checking for kprobe header ---"
	@ls $(KDIR)/include/linux/kprobes.h || echo "error: kprobes.h not found"
	@echo "--- checking symbol to hook ---"
	@grep -w bprm_execve /proc/kallsyms || echo "warning: bprm_execve not found"
