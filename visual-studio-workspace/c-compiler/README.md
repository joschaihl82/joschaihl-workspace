c-compiler (minimal)

Build the compiler and run tests

Build compiler:

```bash
gcc -g cc.c -o cc
```

Generate assembly from a C file:

```bash
./cc path/to/file.c > out.s
gcc -o prog out.s
./prog
```

Run included tests:

```bash
./run_tests.sh
```
