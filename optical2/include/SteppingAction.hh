#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"

class DetectorConstruction;

/// Stepping action class.
///
/// In UserSteppingAction() there are collected the energy deposit and track
/// lengths of charged particles in Absober and Gap layers and
/// updated in RunData object.

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction(const DetectorConstruction* detConstruction, bool analysis=true);
  ~SteppingAction() override = default;

  void UserSteppingAction(const G4Step* step) override;

  static bool analysis;

private:
  const DetectorConstruction* fDetConstruction = nullptr;
};


#endif
