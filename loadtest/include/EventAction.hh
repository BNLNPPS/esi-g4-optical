#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <julia.h>

// ---

class EventAction : public G4UserEventAction {
  public:
    EventAction(); // = default;
    ~EventAction() override = default;

    void  BeginOfEventAction(const G4Event* event)  override;
    void  EndOfEventAction(const G4Event* event)    override;

  private:
    void PrintEventStatistics(G4double absoEdep, G4double absoTrackLength,
                              G4double gapEdep, G4double gapTrackLength) const;

    jl_function_t* begin_event_action_jl=NULL;
    jl_function_t*   end_event_action_jl=NULL;    
};

#endif


