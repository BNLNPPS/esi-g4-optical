#ifndef OpticalMessenger_h
#define OpticalMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4VUserDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAString;

// ---

class OpticalMessenger : public G4UImessenger
{
public:
    OpticalMessenger(G4VUserDetectorConstruction*);
    ~OpticalMessenger() override;
    void SetNewValue(G4UIcommand*, G4String) override;

private:
    G4VUserDetectorConstruction* fDetCon = nullptr;
    G4UIdirectory* fDetConDir = nullptr;
    G4UIcmdWithABool* fVerboseCmd = nullptr;
    G4UIcmdWithABool* fDumpGdmlCmd = nullptr;
    G4UIcmdWithAString* fDumpGdmlFileNameCmd = nullptr;
};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
