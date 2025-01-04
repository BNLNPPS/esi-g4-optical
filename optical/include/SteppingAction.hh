#include "G4UserSteppingAction.hh"
#include "RunAction.hh"
#include "globals.hh"





class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(RunAction*);
    ~SteppingAction() override;
    RunAction *runAction;
    void UserSteppingAction(const G4Step*) override;

};



