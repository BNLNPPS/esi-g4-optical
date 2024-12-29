# Simple Testbed, the "Optical 1"

## Build

It's best to use "out of source" build:

```bash
mkdir build
cmake -B build
cd build/
make
# you can use the -j option to make it work quicked, e.g.
make -j8
```

After that, there will be the executable name _Opt_.
It needs the Geant paths set correctly, to operate.