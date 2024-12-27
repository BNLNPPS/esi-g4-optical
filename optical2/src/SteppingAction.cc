// ---

#include "Steering.hh"

#include "SteppingAction.hh"
#include "RunData.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

#include "G4AnalysisManager.hh"
#include <julia.h>
// --


SteppingAction::SteppingAction(const DetectorConstruction *detConstruction) : fDetConstruction(detConstruction) {
  action_jl = (stepaction_f)(jl_unbox_voidpointer(jl_eval_string("@cfunction(stepping_action, Nothing, (CxxPtr{G4Step},))")));
  std::cout << "=====> " << action_jl << std::endl;
}

// ---
void SteppingAction::UserSteppingAction(const G4Step *step)
{
  action_jl(step);
  if(Steering::callback) {
    // jl_value_t *argument = jl_box_float64(2.0);

  }

  if (not Steering::analysis) {return;}
  //
  G4Track *theTrack = step->GetTrack();
  if (theTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {

    if (theTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov")  {

      double x = step->GetPostStepPoint()->GetPosition().x();
      double y = step->GetPostStepPoint()->GetPosition().y();
      double z = step->GetPostStepPoint()->GetPosition().z();
      double x_dir = step->GetTrack()->GetMomentumDirection()[0];
      double y_dir = step->GetTrack()->GetMomentumDirection()[1];
      double z_dir = step->GetTrack()->GetMomentumDirection()[2];
      double energy = step->GetPostStepPoint()->GetKineticEnergy();


      auto analysisManager = G4AnalysisManager::Instance();
      analysisManager->FillNtupleDColumn(0, x_dir);
      analysisManager->FillNtupleDColumn(1, y_dir);
      analysisManager->FillNtupleDColumn(2, z_dir);
      analysisManager->FillNtupleDColumn(3, energy);
      analysisManager->AddNtupleRow();

      // G4cout << "***" << energy << G4endl;
      theTrack->SetTrackStatus(fStopAndKill);
      auto runData = static_cast<RunData*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
      runData->AddPhoton();
    }

  }
}


// -------------------- ATTIC
      // auto analysisManager = G4AnalysisManager::Instance();
      // if (analysisManager->IsOpenFile())
      // {
      //   // Fill the NTuple
      //   analysisManager->FillNtupleDColumn(0, energy);
      //   analysisManager->FillNtupleDColumn(1, x);
      //   analysisManager->FillNtupleDColumn(2, y);
      //   analysisManager->FillNtupleDColumn(3, z);
      //   analysisManager->FillNtupleDColumn(4, x_dir);
      //   analysisManager->FillNtupleDColumn(5, y_dir);
      //   analysisManager->FillNtupleDColumn(6, z_dir);
      //   analysisManager->AddNtupleRow();
      // }

      /*
      std::vector<double> Photon = {x, y, z, x_dir, y_dir, z_dir, energy};
      runAction->AddPhoton(Photon);
      theTrack->SetTrackStatus(fStopAndKill);
      */


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
