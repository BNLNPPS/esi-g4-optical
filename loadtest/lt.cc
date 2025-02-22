// ---

#include "Steering.hh"

// ---
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "WorkerInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"


// #include "G4VisExecutive.hh"

#include "Randomize.hh"


// important for optical
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "FTFP_BERT.hh"
#include <iostream>
#include "lyra.hpp"
#include <julia.h>

bool Steering::analysis, Steering::callback, Steering::verbose;

JULIA_DEFINE_FAST_TLS // only define this once, in an executable

// -----------------------------------------------------------
using namespace std;
int main(int argc,char** argv) {
  // G4cout << "Start"  << G4endl;

  bool help=false, batch=false, analysis=false, callback=false, verbose=false;
  auto cli = lyra::cli()
    | lyra::opt(batch)                      ["-b"]["--batch"]         ("Optional batch mode").optional()
    | lyra::opt(callback)                   ["-j"]["--julia"]         ("Optional: Julia callback mode").optional()
    | lyra::opt(analysis)                   ["-a"]["--analysis"]      ("Optional: analysis mode").optional()
    | lyra::opt(verbose)                    ["-v"]["--verbose"]       ("Optional: verbose mode").optional()
    | lyra::opt(help)                       ["-h"]["--help"]          ("Help").optional();

  auto result = cli.parse({ argc, argv });
  int8_t state=0;
  
  jl_init(); /* required: setup the Julia context */

  state = jl_gc_safe_enter(jl_current_task->ptls); 

  if (jl_exception_occurred()) {
    cout << "Julia exception: " << jl_typeof_str(jl_exception_occurred()) << endl;
    jl_atexit_hook(0); // exit Julia
    exit(-1);
  }


  //state = jl_gc_safe_enter(jl_current_task->ptls); // Handle the Julia GC mechanics, before execution:
  jl_eval_string("include(\"./julia/custom_module.jl\")");
  jl_eval_string("using .custom");


  jl_function_t* test_jl = jl_get_function(jl_main_module, "test_func");

  if (test_jl == NULL) {
    cout << "MAIN --  test_jl is null, exiting..." << endl;
    jl_atexit_hook(0);
    exit(0);
  }
  else {
    cout << "MAIN -- test load of Julia successful" << endl;
  }

  cout << "MAIN -- Calling test_func " << endl;
  
  jl_value_t *tf = jl_call0(test_jl); // just a "ping"

  int tF = jl_unbox_int8(tf);
  cout << "MAIN -- return from test_func... " <<  tF << endl;

  //  jl_eval_string("import Plots;  println(\"foo\");  img = plot(rand(1000));"); // savefig(img, \"f.png\")

  if (jl_exception_occurred()) {
    cout << "Julia exception: " << jl_typeof_str(jl_exception_occurred()) << endl;
    jl_atexit_hook(0); // exit Julia
    exit(-1);
  }

  jl_function_t* plotter = jl_get_function(jl_main_module, "plotter");

  if (plotter == NULL) {
    cout << "MAIN --  plotter is null, exiting..." << endl;
    jl_atexit_hook(0);
    exit(0);
  }
  else {
    cout << "MAIN -- plotter load of Julia successful" << endl;
  }

  jl_call0(plotter);

}
