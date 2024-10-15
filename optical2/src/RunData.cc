#include "RunData.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"

// ---
void RunData::FillPerEvent() {
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillH1(0, nPhotons);
  G4cout << "Filling H1 in RunData:" << nPhotons << G4endl;

  // // accumulate statistic  in the order of the histograms, ntuple columns declarations
  // G4int counter = 0;
  // for ( auto edep : fEdep ) {
  //   analysisManager->FillH1(counter, edep);
  //   analysisManager->FillNtupleDColumn(counter++, edep);
  // }
  // for ( auto trackLength : fTrackLength ) {
  //   analysisManager->FillH1(counter, trackLength);
  //   analysisManager->FillNtupleDColumn(counter++, trackLength);
  // }
  
  // analysisManager->AddNtupleRow();
}

// ---

void RunData::Reset() {
  nPhotons=0;

  // for ( auto& edep : fEdep ) {edep = 0.;}
  // for ( auto& trackLength : fTrackLength ) {trackLength = 0.;}

}

