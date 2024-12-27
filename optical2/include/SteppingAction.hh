#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"

class DetectorConstruction;

typedef  void (*stepaction_f)(const G4Step*);


class SteppingAction : public G4UserSteppingAction {
public:
  SteppingAction(const DetectorConstruction* detConstruction);
  ~SteppingAction() override = default;

  // void UserSteppingAction(const G4Step* step) override;
  void UserSteppingAction(const G4Step* step) override;// { action_jl(step); }  

private:
  const DetectorConstruction* fDetConstruction = nullptr;
  stepaction_f action_jl;
};


#endif
