#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "G4MTRunManager.hh"
#include "DetectorConstruction.hh"

// ---
ActionInitialization::ActionInitialization(DetectorConstruction* detConstruction)
 : _DetConstruction(detConstruction) {}

// ---
void ActionInitialization::BuildForMaster() const {
  RunAction* rA = new RunAction;
  rA->SetFilename("my.root");
  SetUserAction(rA);
}

// ---
void ActionInitialization::Build() const {
  SetUserAction(new PrimaryGeneratorAction);

  RunAction* rA = new RunAction;
  rA->SetFilename("my.root");
  SetUserAction(rA);

  //  SetUserAction(new RunAction);

  SetUserAction(new EventAction);
  SetUserAction(new SteppingAction(_DetConstruction));
}
