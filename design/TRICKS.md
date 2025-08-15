# List of Tricks to Explore for Reducing Size

## Graphics

Methods for opening a window and getting a GL context:

- X11
- GTK
- SDL(2)
- Raw DRM (?) (brittle)

https://github.com/blackle/Linux-OpenGL-Examples/

### Talking to X11 at a Low Level

Establish a connection to `localhost:6000`, send the initial request, grab the id’s you require from the response, open a window, etc. etc. It may be a bit long-winded, but you also get a low-level access to extensions such as XVideo and GLX. However, if you want access to the GPU (and don’t want to code your own GPU drivers), this isn’t exactly an option as the GPU drivers need to be loaded from dynamic libraries.

## Audio

Audio output can be achieved easily by creating a pipe (using the `pipe` syscall), `fork`ing, `dup2` in the read end of the pipe to `stdin` and `execve`ing the child to `/usr/bin/aplay`. Then, the parent process can simply write PCM data to the pipe, and sound will appear. [Here](https://www.pouet.net/prod.php?which=70107) is a prod that does just this ([source code](https://github.com/Suricrasia-Online/ultra.VIOLET)).

### Synths

- [pl_synth](https://github.com/phoboslab/pl_synth)
- [4klang](https://www.pouet.net/prod.php?which=53398)
- [ForkedKlang](https://www.pouet.net/topic.php?which=11312)
- [Clinkster](https://www.pouet.net/prod.php?which=61592)
- [Oidos](https://www.pouet.net/prod.php?which=69524)
- [ghostsyn](https://github.com/Juippi/ghostsyn)

[PC intro synth examples](https://gitlab.com/PoroCYon/4klang-linux)

## Compression

Standard xz files have large headers compared to gzip. Therefore, it’s a better idea to use xz in LZMA1 mode (xz --lzma1), which has slightly worse compression, but much smaller headers. With small sizes, LZMA1 might perform slightly better than LZMA2 because of this.

https://gitlab.com/-/snippets/1800243 - a script that
brute-forces all xz compression parameters to find the optimal binary

## Self-Compiling

Distribute the game as an executable source code package that decompresses, compiles, executes then deletes the actual game. A shell script stub attached to the compressed source code of an SDL-based game would look something like this:

```sh
a=/tmp/I;zcat<<X>$a;cc `sdl-config --libs --cflags` -o $a. $a;$a.;rm $a.;exit
```

- The code density in C source code is not as good as, say, x86 machine code. However, C source code (plain text) compresses better than x86 assembly.
- The compilation may fail for a multitude of reasons that may depend on your distribution, headers, compiler version etc. Standard Ubuntu systems do NOT come with development headers!
- You need either to write the code in a highly compact manner (avoid newline etc) or to write a postprocessor that minimizes the code for you.

## Bypassing Startup Code

Basically, the `main` function you wrote isn’t the *real* first thing that gets executed. The very first code that actually runs in the process – ignoring the dynamic linker for a moment – can be found in the compiler’s `crt*` files. Those files fetch the `argc`, `argv`, environment variables and the [ELF auxiliary vector](https://www.youtube.com/watch?v=j72GExsDnDU&t=1015) from the kernel, align the stack, initialize the internal libc components, and only then it jumps to `main`. (For more details, visit the Linux Sizecoding Wiki.)

However, this code is rather large, and it’s easy to bypass this: `-nostartfiles`. But, if you use this flag without thinking, you’ll get another error: `“function _start not found”`. This is the real entrypoint, so just rename your `main` to `_start`. Then it will run, but your `argc` and `argv` and `environ` will be garbage. Also, you might notice **random crashes in random places that use SSE instructions**.

This means, you’ll have to initialize and align the stack yourself. Fortunately, this isn’t too hard: `argc` and `argv` etc. are placed on the stack at process entry as shown in the ASCII diagram in the midddle of [this article](https://lwn.net/Articles/631631/). Then call `__libc_start_main` with `main`, `argc`, `argv`, three null pointers, and finally the initial stack value. An example of doing this can be found [here](https://github.com/Shizmob/smol/blob/master/rt/crt1.c), except fetching the stack pointer value is left as an exercise for the reader. (Just `mov foo`, `esp`/`rsp` isn’t a solution because the compiler inserts a function prologue.)

## Compiler/Linker Tools

https://github.com/PoroCYon/smol

https://github.com/faemiyah/dnload/

# Sources

- https://in4k.github.io/wiki/linux
- https://in4k.github.io/wiki/linux-static
- https://in4k.github.io/html_articles/A%20Whirlwind%20Tutorial%20on%20Creating%20Really%20Teensy%20ELF%20Executables%20for%20Linux.html
- https://twiren.kapsi.fi/linux4k.html
- https://files.scene.org/view/resources/in4k/linux_4k_intro_coding_asm06.pdf
