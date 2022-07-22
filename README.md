# tetric
Tetris with better graphics

# Building
Requires cmake

If on macos, you may need to remove `-Werror` from `CFLAGS` in the Makefile.

```
git clone --recurse-submodules https://github.com/longwatermelon/tetric
cd tetric
make deps
make
./a.out
```
