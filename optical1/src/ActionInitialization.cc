#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"

// ---
ActionInitialization::ActionInitialization()
 : G4VUserActionInitialization()
{}

// ---
ActionInitialization::~ActionInitialization()
{}

// ---
void ActionInitialization::BuildForMaster() const
{}


// ---
void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction);
  RunAction* fRunAct = new RunAction();
  
  SetUserAction(fRunAct);
  SetUserAction(new SteppingAction(fRunAct));

}



