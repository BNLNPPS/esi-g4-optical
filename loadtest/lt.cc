// ---
#include <iostream>
#include <julia.h>
#include "lyra.hpp"

JULIA_DEFINE_FAST_TLS // only define this once, in an executable

// -----------------------------------------------------------

using namespace std;

int main(int argc,char** argv) {

  jl_init(); /* required: setup the Julia context */

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
