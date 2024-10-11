#include "SteppingAction.hh"
#include "RunData.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"


// ---
SteppingAction::SteppingAction(const DetectorConstruction* detConstruction): fDetConstruction(detConstruction) {}

// ---
void SteppingAction::UserSteppingAction(const G4Step* step) {

  // -mxp- using logic in "optical1"
  G4Track* theTrack = step->GetTrack();
  if(theTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()) {
    G4cout << "+" << G4endl;
  }



  // -mxp- begin comment previous content

  // Collect energy and track length step by step   // get volume of the current step
  // auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

  // // energy deposit
  // auto edep = step->GetTotalEnergyDeposit();

  // // step length
  // G4double stepLength = 0.;
  // if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
  //   stepLength = step->GetStepLength();
  // }

  // auto runData = static_cast<RunData*>
  //   (G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  // if ( volume == fDetConstruction->GetAbsorberPV() ) {
  //   runData->Add(kAbs, edep, stepLength);
  // }

  // if ( volume == fDetConstruction->GetGapPV() ) {
  //   runData->Add(kGap, edep, stepLength);
  // }
  // -mxp- end comment

}
