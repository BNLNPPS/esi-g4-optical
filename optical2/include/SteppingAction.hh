#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"

class DetectorConstruction;


class SteppingAction : public G4UserSteppingAction {
public:
  SteppingAction(const DetectorConstruction* detConstruction);
  ~SteppingAction() override = default;

  void UserSteppingAction(const G4Step* step) override;

private:
  const DetectorConstruction* fDetConstruction = nullptr;
};


#endif
