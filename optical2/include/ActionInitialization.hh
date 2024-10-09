#ifndef B4aActionInitialization_h
#define B4aActionInitialization_h 1

#include "G4VUserActionInitialization.hh"


namespace B4
{
  class DetectorConstruction;
}

namespace B4b
{

/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(B4::DetectorConstruction*);
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;

  private:
    B4::DetectorConstruction* fDetConstruction = nullptr;
};

}

#endif


