# tetric
Tetris with better graphics

# Building
Requires cmake

Probably will work on linux, support is iffy on macOS, definitely won't work on windows.

```
git clone --recurse-submodules https://github.com/longwatermelon/tetric
cd tetric
make deps
make
./a.out
```

# Controls
* Left / right keys: move piece
* Up: rotate
* Down: soft drop
* Space: hard drop
* c: hold
* g: toggle normal mapping
* r: rotate around board
