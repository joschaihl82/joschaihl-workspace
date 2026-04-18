savedcmd_exectrace1.mod := printf '%s\n'   exectrace1.o | awk '!x[$$0]++ { print("./"$$0) }' > exectrace1.mod
