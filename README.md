# A Teeny Tiny compiler in C++

It compiles (transpiles) our own dialect of BASIC to C.

## Build

Run

```shell
make
```

## Usage

This will generate `teenytinyc` compiler binary.
You can use it to compile Teeny Tiny language to C.

Like so:

```
./teenytinyc examples/fibonacci.teeny
```

It outputs the C code to `out.c`.
You may use `clang-format` to inpect the resulting C code.
You may use any C compiler to get the program binary.

Like so:

```shell
gcc -o fibonacci out.c
```

Then just run the program

```shell
./fibonacci
```

There is a helper script `build.sh`, which runs the `teenytinyc`
compiler and the C compiler to get the binary.

```shell
./build.sh examples/fibonacci.teeny
```

## References

- [Let's make a Teeny Tiny compiler, part 1](https://austinhenley.com/blog/teenytinycompiler1.html)
- [Let's make a Teeny Tiny compiler, part 2](https://austinhenley.com/blog/teenytinycompiler2.html)
- [Let's make a Teeny Tiny compiler, part 3](https://austinhenley.com/blog/teenytinycompiler3.html)
