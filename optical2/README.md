# optical2

## EventAction and RunData

In the beginning of each event, the photon counter is reset to zero.

## SteppingAction and RunData

`SteppingAction` kills the Cherenkov photon and calls `RunData` to increment the photon counter.

# Building

In the _build_ folder, it is convenient to run:

```bash
cmake ../ -B . && make -j6
```
