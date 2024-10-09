#include "SteppingAction.hh"
#include "RunData.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

using namespace B4;

namespace B4b {

// ---
SteppingAction::SteppingAction(const DetectorConstruction* detConstruction): fDetConstruction(detConstruction) {}

// ---
void SteppingAction::UserSteppingAction(const G4Step* step) {
  // Collect energy and track length step by step
  // get volume of the current step
  auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  auto runData = static_cast<RunData*>
    (G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  if ( volume == fDetConstruction->GetAbsorberPV() ) {
    runData->Add(kAbs, edep, stepLength);
  }

  if ( volume == fDetConstruction->GetGapPV() ) {
    runData->Add(kGap, edep, stepLength);
  }
}

}
