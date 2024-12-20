#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "G4MTRunManager.hh"
#include "DetectorConstruction.hh"

#include <julia.h>

// JULIA_DEFINE_FAST_TLS

// ---
ActionInitialization::ActionInitialization(DetectorConstruction* detConstruction, G4String fn):_DetConstruction(detConstruction), _FileName(fn) {};

// ---
void ActionInitialization::BuildForMaster() const {
  RunAction* rA = new RunAction(_FileName);
  SetUserAction(rA);


  G4cout << "Master Action output file: "  << _FileName << " " << _FileName.size() << G4endl;  

  jl_eval_string("println(sqrt(2.0))");
  jl_function_t *test_func = jl_get_function(jl_main_module, "test_func");

  if (test_func == NULL) {
      G4cout << "Action initialization --  test_func is null, exiting..." << G4endl;      
      jl_atexit_hook(0);
      exit(0);
  }

  G4cout << "Calling test_func " << G4endl;
  jl_call0(test_func);


  jl_value_t *argument = jl_box_float64(2.0);
  jl_function_t *operation = jl_get_function(jl_main_module, "operation");

  jl_value_t *op_ret = jl_call1(operation, argument);

  if (jl_typeis(op_ret, jl_float64_type)) {
      double ret_unboxed = jl_unbox_float64(op_ret);
      G4cout << "op_ret in C: " <<  ret_unboxed << G4endl;
  }
  else {
      G4cout << "ERROR: unexpected return type from op_ret" << G4endl;
  }



}

// ---
void ActionInitialization::Build() const {
  SetUserAction(new PrimaryGeneratorAction);

  RunAction* rA = new RunAction(_FileName);
  SetUserAction(rA);

  G4cout << "Worker Action output file: "  << _FileName  << G4endl;  

  SetUserAction(new EventAction);
  SetUserAction(new SteppingAction(_DetConstruction, _FileName.length()>0));
}


// -- ATTIC
//   jl_value_t *ret1 = jl_call1(func, argument);
//   if (jl_typeis(ret1, jl_float64_type)) {
//       double ret_unboxed = jl_unbox_float64(ret1);
//       printf("sqrt(2.0) in C: %e \n", ret_unboxed);
//   }
//   else {
//       printf("ERROR: unexpected return type from sqrt(::Float64)\n");
//   }