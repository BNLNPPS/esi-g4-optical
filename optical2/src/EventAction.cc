#include "EventAction.hh"
#include "RunData.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "G4AnalysisManager.hh"

#include "Randomize.hh"
#include <iomanip>

// ---
void EventAction::PrintEventStatistics(
                              G4double absoEdep, G4double absoTrackLength,
                              G4double gapEdep, G4double gapTrackLength) const
{  
  // print event statistics
  G4cout
     << "   Absorber: total energy: "
     << std::setw(7) << G4BestUnit(absoEdep, "Energy")
     << "       total track length: "
     << std::setw(7) << G4BestUnit(absoTrackLength, "Length")
     << G4endl
     << "        Gap: total energy: "
     << std::setw(7) << G4BestUnit(gapEdep, "Energy")
     << "       total track length: "
     << std::setw(7) << G4BestUnit(gapTrackLength, "Length")
     << G4endl;
}

// ---
void EventAction::BeginOfEventAction(const G4Event* /*event*/) {

  auto runData = static_cast<RunData*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  runData->Reset();
}

// ---
void EventAction::EndOfEventAction(const G4Event* event) {
  //        auto analysisManager = G4AnalysisManager::Instance();
  auto runData = static_cast<RunData*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  auto analysisManager = G4AnalysisManager::Instance();

  G4cout << "Open file:"  << analysisManager->IsOpenFile()  << G4endl;

  G4int N = runData->GetNphotons();
  G4cout << "Photons in Event:" << N << G4endl;

  runData->FillPerEvent();
}

