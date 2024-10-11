#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

// In EndOfEventAction(), it prints the accumulated quantities of the energy
// deposit and track lengths of charged particles in Absober and Gap layers
// stored in RunData object.

class EventAction : public G4UserEventAction {
  public:
    EventAction() = default;
    ~EventAction() override = default;

    void  BeginOfEventAction(const G4Event* event)  override;
    void  EndOfEventAction(const G4Event* event)    override;

  private:
    void PrintEventStatistics(G4double absoEdep, G4double absoTrackLength,
                              G4double gapEdep, G4double gapTrackLength) const;
};

#endif


