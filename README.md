# The ESI Optical Testbed

## About

This repository is meant for testing a variety of the Geant4 functionality
related to optical processes, mainly Cherenkov photon generation and propagation.

## Building

It strongly recommended to use the "out of source" build pattern. For example,
in the folder **optical1** one first creates a folder named "build", then
runs CMake and finally the build itself using _make_, from the build folder itself.
This can look as follows:

```bash
cd optical1
mkdir build
cmake -B build
cd build/
make
# you can use the -j option to make it work quicked, e.g.
make -j8
```

## OpenGL under WSL2

To get graphics working when running under WSL2, it may be necessary to apply this setting:
```bash
export LIBGL_ALWAYS_INDIRECT=
```

## Outline of the Geant logic and workflow

### Main

* Create the _G4UIExecutive_ and the _runManager_
* Initialize the _runManager_ with the _DetectorConstruction_
* Create the _physicsList_ which includes optical physics
* Call _ActionInitialization_

### PrimaryGeneratorAction

This is created in the "Build" method of _ActionInitialization_. Various particle generation
parameters are stored here, as well as a reference to the _ParticleGun_.

### RunAction

Generates the "Run" object, and calls "EndOfRun" in the end.


