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


  G4cout << "Master Action output file: "  << _FileName << G4endl;  

  jl_eval_string("println(sqrt(2.0))");
  jl_function_t *func= jl_get_function(jl_main_module, "func");

  if (func != NULL) {
      printf("Action initialization -- func is not null\n");
  }
  else {
      printf("Action initialization --  func is null\n");
      jl_atexit_hook(0);
      exit(0);
  }

  jl_call0(func);

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
