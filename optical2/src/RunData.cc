#include "RunData.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

#include "SteppingAction.hh"

// ---

// RunData::RunData() {

// }

void RunData::FillPerEvent() {
  G4cout << "Stepping action analysis: " << SteppingAction::analysis << G4endl;
  if (not SteppingAction::analysis) {
    G4cout << "Not Filling H1 in RunData: analysis inactive" << G4endl;
    return;
  }
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillH1(0, nPhotons);
  G4cout << "Filling H1 in RunData:" << nPhotons << G4endl;
}

// ---

void RunData::AddPhoton() {
  nPhotons+=1;
}

void RunData::Reset() {
  nPhotons=0;
}

