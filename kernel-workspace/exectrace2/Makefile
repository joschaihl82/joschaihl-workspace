# 3.5 rule: mod prefix, obj/nam/dir/sym postfix
mod_obj := exectrace1.o
mod_nam := exectrace1
mod_dir := /lib/modules/$(shell uname -r)/extra
mod_sym := bprm_execve
obj-m := $(mod_obj)

# 3.5 rule: mak prefix, bin/knd/cur/flg postfix
mak_bin := make
mak_knd := /lib/modules/$(shell uname -r)/build
mak_cur := $(shell pwd)
mak_flg := v=1

# 3.5 rule: chk prefix, hdr/kpb postfix
chk_hdr := $(mak_knd)/include/linux/kprobes.h
chk_kpb := /proc/kallsyms

.phony: all clean cln_all load unload debug

all:
	$(mak_bin) -C $(mak_knd) M=$(mak_cur) modules $(mak_flg)

clean:
	$(mak_bin) -C $(mak_knd) M=$(mak_cur) clean

cln_all: clean
	rm -f .*.cmd *.o *.mod *.mod.c modules.order Module.symvers

load: all
	sudo mkdir -p $(mod_dir)
	sudo cp $(mod_nam).ko $(mod_dir)
	sudo depmod -a
	sudo modprobe $(mod_nam) || (sudo dmesg | tail -n 20 && exit 1)

unload:
	sudo modprobe -r $(mod_nam)
	sudo rm -f $(mod_dir)/$(mod_nam).ko
	sudo depmod -a

debug:
	@echo "--- checking for kprobe header ---"
	@ls $(chk_hdr) || echo "error: kprobes.h not found in $(mak_knd)"
	@echo "--- checking symbol to hook ---"
	@grep -w $(mod_sym) $(chk_kpb) || echo "warning: $(mod_sym) not found"
