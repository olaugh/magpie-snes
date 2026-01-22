# sxtest Vendor Dependencies

This directory contains prebuilt dependencies for sxtest.

## snes9x libretro core

The `snes9x/libretro/` directory contains:
- `snes9x_libretro.a` - Prebuilt static library for macOS ARM64
- `libretro.h` - Libretro API header

### Rebuilding the library

If you need to rebuild the snes9x libretro core:

```bash
# Clone snes9x
git clone https://github.com/snes9xgit/snes9x.git vendor/snes9x

# Build static library for macOS
cd vendor/snes9x/libretro
make clean
make platform=osx STATIC_LINKING=1

# The output will be snes9x_libretro.a
```

### Platform support

The prebuilt library is for macOS ARM64 (Apple Silicon). For other platforms:

- **macOS x86_64**: Use `make platform=osx STATIC_LINKING=1`
- **Linux**: Use `make STATIC_LINKING=1`
- **Windows**: Use MSVC project in `msvc/` directory

### License

snes9x is licensed under a custom license. See the snes9x repository for details.
