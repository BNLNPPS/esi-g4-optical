#include "EventAction.hh"
#include "RunData.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

namespace B4b
{

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

  auto runData
    = static_cast<RunData*>(
        G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  runData->Reset();
}

// ---
void EventAction::EndOfEventAction(const G4Event* event) {
  auto runData = static_cast<RunData*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  // -mxp- begin comment
  // runData->FillPerEvent();


  // auto eventID = event->GetEventID();
  // auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
 
  // if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
  //   PrintEventStatistics(
  //     runData->GetEdep(kAbs),
  //     runData->GetTrackLength(kAbs),
  //     runData->GetEdep(kGap),
  //     runData->GetTrackLength(kGap));
  //   G4cout << "--> End of event " << eventID << "\n" << G4endl;      
  // }
  // -mxp- end comment
}

}
