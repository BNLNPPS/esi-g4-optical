# Julia embedded in Geant4 applications

## "main"

The driver of the application ("main") can handle a variety of command line
options. These can be listed using the `--help` option. By default, the program
will execute a simple initialization sequence witih the default particle and energy,
and will proceed to run 100 events. The number of events can be modified from the command
line via the appropriate option. Alternatively, the user may specify the macro file
to be executed.

## ActionInitialization

There are two methods, `Build` and `BuildForMaster`, reflecting the difference between threads.
This is important for two reasons:
* to ensure procedures like _NTuple_ merging and others work correctly
* to handle special cases like _Julia callbacks_

## EventAction and RunData

In the beginning of each event, the photon counter is reset to zero.

## SteppingAction and RunData

* `SteppingAction` kills the Cherenkov photon and calls `RunData` to increment the photon counter.
* Also, Julia callbacks are optionally added to `SteppingAction`


## RunAction

Calls _Analysis Manager_ to open the file. May contain Julia callbacks for general testing.


---

# Technical

## Building

In the _build_ folder, it is convenient to run:

```bash
cmake ../ -B . && make -j6
```

## Running

Current name of the executable is _Opt2_, and the command line options are as follows:

```bash
USAGE:
  Opt2 [-o|--output_file <output_file>] [-m|--macro <macro>] [-n|--nevents <nevents>] [-t|--threads <threads>] [-b|--batch] [-j|--julia] [-a|--analysis] [-v|--verbose] [-h|--help]

OPTIONS, ARGUMENTS:
  -o, --output_file <output_file>
                          Output file, default empty
  -m, --macro <macro>     Optional macro
  -n, --nevents <nevents> Optional: number of events  [default: 100]
  -t, --threads <threads> Optional: number of threads [default: 0]
  -b, --batch             Optional batch mode
  -j, --julia             Optional: Julia callback mode
  -a, --analysis          Optional: analysis mode
  -v, --verbose           Optional: verbose mode
  -h, --help              Help
```

## Julia

### Environment for CMake
The CMake in this project will need a setting like:
```bash
export JULIA_DIR=/opt/julia-1.11.2/
```
### GC Thread safety

Note that before the execution in the "main", one  needs to enter the MT safe mode for Julia GC:
```bash
auto state = jl_gc_safe_enter(jl_current_task->ptls);
```

...and upon completion, exit the mode:
```bash
jl_gc_safe_leave(jl_current_task->ptls, state);
```
This functions are a part of the Julia library required to build this application.


## Graphics under WSL2

```bash
export LIBGL_ALWAYS_INDIRECT=
```

