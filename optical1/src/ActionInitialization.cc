#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include <string>

// ---
ActionInitialization::ActionInitialization(std::string OutputFormat)
 : G4VUserActionInitialization()
{
_OutputFormat = OutputFormat;

}

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
  RunAction* fRunAct = new RunAction(nullptr, _OutputFormat);
  
  SetUserAction(fRunAct);
  SetUserAction(new SteppingAction(fRunAct));

}



