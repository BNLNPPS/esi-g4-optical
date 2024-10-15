#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "G4String.hh"

class DetectorConstruction;

/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization {
  public:
    ActionInitialization(DetectorConstruction*, G4String fn);
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;

  private:
    DetectorConstruction* _DetConstruction  = nullptr;
    G4String              _FileName         = "my.root";
};

#endif


