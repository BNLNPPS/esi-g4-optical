# optical2

## EventAction and RunData

In the beginning of each event, the photon counter is reset to zero.

## SteppingAction and RunData

`SteppingAction` kills the Cherenkov photon and calls `RunData` to increment the photon counter.


## RunAction

Calls _Analysis Manager_ to open the file.


---

# Technical

## Building

In the _build_ folder, it is convenient to run:

```bash
cmake ../ -B . && make -j6
```


## Julia bindings

The CMake in this project will need a setting like:
```bash
export JULIA_DIR=/opt/julia-1.11.2/
```

## Graphics under WSL2

```bash
export LIBGL_ALWAYS_INDIRECT=
```

