/*
 * main.c - main program
 *
 * Copyright 2010 Rui Ueyama <rui314@gmail.com>.  All rights reserved.
 * This code is available under the simplified BSD license.  See LICENSE for details.
 */

#include "8cc.h"

static void usage(void) {
    fprintf(stderr,
            "Usage: 8cc [ -d ] [ -run ] <infile>\n"
            "       8cc [ -E ] <infile>\n"
            "       8cc [ -d ] <infile> <outfile>\n");
    exit(-1);
}



/* main: if invoked as a linker (first arg is -o) delegate to ld(),
 * otherwise run the normal 8cc compile/assemble flow.
 */
int main(int argc, char **argv) {
    eightcc_init();

    /* If the program is invoked as a linker: ld style:  prog -o output in1.o in2.o ...
     * delegate to the ld() function which implements the linking flow.
     */
    if (argc >= 2 && strcmp(argv[1], "-o") == 0) {
        return ld(argc, argv);
    }

    bool flag_cscript = false;
    bool flag_cpp_only = false;
    char *infile = NULL;
    char *outfile = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-run"))
            flag_cscript = true;
        else if (!strcmp(argv[i], "-d"))
            flag_debug = true;
        else if (!strcmp(argv[i], "-E"))
            flag_cpp_only = true;
        else if (argv[i][0] == '-' && argv[i][1] != '\0')
            usage();
        else if (!infile) {
            infile = argv[i];
        }
        else if (!outfile)
            outfile = argv[i];
        else
            usage();
    }

    if (!infile)
        usage();
    if ((flag_cscript || flag_cpp_only) && outfile)
        usage();
    if (!(flag_cscript || flag_cpp_only) && !outfile)
        usage();

    File *in = open_file(infile);
    Elf *elf = new_elf();

    if (flag_cpp_only) {
        cpp_write(make_cpp_context(in), stdout);
        return 0;
    }

    List *fns = parse(in, elf);
    assemble(elf, fns);

    if (flag_cscript) {
        run_main(elf, argc - 1, argv + 1);
        return 0;
    } else {
        /* Write the assembled ELF to a temporary object file, then call ld()
         * to link it into the requested outfile.
         */
        char tmpobj[] = "/tmp/8cc_objXXXXXX";
        int fd = mkstemp(tmpobj);
        if (fd == -1) {
            perror("mkstemp");
            return 1;
        }

        FILE *out = fdopen(fd, "w");
        if (!out) {
            perror("fdopen");
            close(fd);
            unlink(tmpobj);
            return 1;
        }

        write_elf(out, elf);
        fclose(out); /* closes fd as well */

        /* Build argv for ld: program -o outfile tmpobj */
        char *ld_argv[4];
        ld_argv[0] = argv[0];
        ld_argv[1] = "-o";
        ld_argv[2] = outfile;
        ld_argv[3] = tmpobj;
        int ld_argc = 4;

        int rc = ld(ld_argc, ld_argv);

        /* Clean up temporary object file */
        unlink(tmpobj);

        return rc;
    }

    /* unreachable */
    return 0;
}

