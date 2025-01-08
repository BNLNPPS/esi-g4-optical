// ---

#include "Steering.hh"

#include "EventAction.hh"
#include "RunData.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "G4AnalysisManager.hh"

#include "Randomize.hh"
#include <iomanip>

// ---
EventAction::EventAction() {
  begin_event_action_jl = jl_get_function(jl_main_module, "begin_event");
  if (begin_event_action_jl == NULL) {
      G4cout << "Event Action ctor --  begin_event_action_jl is null, exiting..." << G4endl;
      jl_atexit_hook(0);
      exit(0);
    }
}

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
void EventAction::BeginOfEventAction(const G4Event*) {
  auto runData = static_cast<RunData*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  auto id = G4Threading::G4GetThreadId();
  
  jl_value_t *argument = jl_box_int8(id);
  jl_value_t *thread_ret = jl_call1(begin_event_action_jl, argument);
  int thread_ret_unboxed = jl_unbox_int8(thread_ret);  

  G4cout << "=====> Begin Event <=========== thread id: " << thread_ret_unboxed << G4endl;
  // jl_call0(begin_event_action_jl);

  runData->Reset();
}

// ---
void EventAction::EndOfEventAction(const G4Event* event) {
  //        auto analysisManager = G4AnalysisManager::Instance();
  auto runData = static_cast<RunData*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());

  auto analysisManager = G4AnalysisManager::Instance();

  // G4cout << "Open file:"  << analysisManager->IsOpenFile()  << G4endl;

  if(Steering::verbose) {
    G4int N = runData->GetNphotons();
    G4cout << "Photons in Event:" << N << G4endl;
  }

  runData->FillPerEvent();
}

