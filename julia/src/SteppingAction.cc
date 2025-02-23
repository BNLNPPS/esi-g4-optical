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
// --

SteppingAction::SteppingAction(const DetectorConstruction *detConstruction) : fDetConstruction(detConstruction)
{

  action_jl = jl_get_function(jl_main_module, "stepping_action");
  if (action_jl == NULL) {
    G4cout << "Stepping Action ctor --  action_jl is null." << G4endl;
    // jl_atexit_hook(0);
    // exit(0);
  }
}

// ---
void SteppingAction::UserSteppingAction(const G4Step *step) {
  if (Steering::callback) {

    G4Track *theTrack = step->GetTrack();

    const G4DynamicParticle *particle     = theTrack->GetDynamicParticle();

    const G4StepPoint *preStepPoint       = step->GetPreStepPoint();
    const G4StepPoint *postStepPoint      = step->GetPostStepPoint();

    const G4TouchableHandle &preStepTouch = preStepPoint->GetTouchableHandle();
    const G4VPhysicalVolume *volume       = preStepTouch->GetVolume();
    const G4LogicalVolume *lVolume        = volume->GetLogicalVolume();

    G4String vname = volume->GetName();

    if (vname == "detectorPhysical" && action_jl) {

      G4double ed = step->GetTotalEnergyDeposit();

      auto id = G4Threading::G4GetThreadId();
      jl_value_t *thread_boxed  = jl_box_int8(id);
      jl_value_t *ed_boxed      = jl_box_float64(ed);

      jl_value_t *thread_ret = jl_call2(action_jl, thread_boxed, ed_boxed);
      float thread_ret_unboxed = jl_unbox_float64(thread_ret);

      // G4cout << ed << G4endl;
    }
  }

  if (not Steering::analysis) {
    return;
  }
  
  // ---
  G4Track *theTrack = step->GetTrack();
  if (theTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {
    if (theTrack->GetCreatorProcess()->GetProcessName() == "Cerenkov") {

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
      auto runData = static_cast<RunData *>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
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

// This is the original callback created by Pere, but we don't need full functionaltiy yet.
// action_jl = (stepaction_f)(jl_unbox_voidpointer(jl_eval_string("@cfunction(stepping_action, Nothing, (CxxPtr{G4Step},))")));
// std::cout << "=====> " << action_jl << std::endl;