#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "G4MTRunManager.hh"
#include "DetectorConstruction.hh"

// ---
ActionInitialization::ActionInitialization(DetectorConstruction* detConstruction, G4String fn):_DetConstruction(detConstruction), _FileName(fn) {};

// ---
void ActionInitialization::BuildForMaster() const {
  RunAction* rA = new RunAction;
  rA->SetFilename(_FileName);
  SetUserAction(rA);
}

// ---
void ActionInitialization::Build() const {
  SetUserAction(new PrimaryGeneratorAction);

  RunAction* rA = new RunAction;
  rA->SetFilename(_FileName);
  SetUserAction(rA);

  SetUserAction(new EventAction);
  SetUserAction(new SteppingAction(_DetConstruction));
}
