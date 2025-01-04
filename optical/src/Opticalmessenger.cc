#include "OpticalMessenger.hh"
#include "DetectorConstruction.hh"
// #include "OpNoviceGDMLDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpticalMessenger::OpticalMessenger(
  G4VUserDetectorConstruction* detcon)
  : G4UImessenger()
  , fDetCon(detcon)
{
  fDetConDir = new G4UIdirectory("/Optical/DetectorConstruction/");
  fDetConDir->SetGuidance("Configuring Detector Construction");

  fVerboseCmd =
    new G4UIcmdWithABool("/Optical/DetectorConstruction/enableVerbose", this);
  fVerboseCmd->SetGuidance("Set flag for enabling verbose diagnostic printout");
  fVerboseCmd->SetDefaultValue(false);
  fVerboseCmd->AvailableForStates(G4State_PreInit);

  fDumpGdmlCmd =
    new G4UIcmdWithABool("/Optical/DetectorConstruction/dumpGdml", this);
  fDumpGdmlCmd->SetGuidance("Set flag for enabling dumping the detector to a gdml file");
  fDumpGdmlCmd->SetDefaultValue(false);
  fDumpGdmlCmd->AvailableForStates(G4State_PreInit);

  fDumpGdmlFileNameCmd = new G4UIcmdWithAString(
    "/Optical/DetectorConstruction/dumpGdmlFileName", this);
  fDumpGdmlFileNameCmd->SetGuidance("Enter file name to dump gdml file ");
  fDumpGdmlFileNameCmd->SetDefaultValue("Optical_dump.gdml");
  fDumpGdmlFileNameCmd->AvailableForStates(G4State_PreInit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

OpticalMessenger::~OpticalMessenger()
{
  delete fDetConDir;
  delete fVerboseCmd;
  delete fDumpGdmlCmd;
  delete fDumpGdmlFileNameCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void OpticalMessenger::SetNewValue(G4UIcommand* command,
                                            G4String newValue)
{
//   auto dc1 = dynamic_cast<DetectorConstruction*>(fDetCon);
//   if(dc1 != nullptr)
//   {
//     if(command == fVerboseCmd)
//       dc1->SetVerbose(fVerboseCmd->GetNewBoolValue(newValue));
//     if(command == fDumpGdmlCmd)
//       dc1->SetDumpGdml(fDumpGdmlCmd->GetNewBoolValue(newValue));
//     if(command == fDumpGdmlFileNameCmd)
//       dc1->SetDumpGdmlFile(newValue);
//   }
//   else
//   {
//     auto dc2 = dynamic_cast<OpNoviceGDMLDetectorConstruction*>(fDetCon);
//     if(command == fVerboseCmd)
//       dc2->SetVerbose(fVerboseCmd->GetNewBoolValue(newValue));
//     if(command == fDumpGdmlCmd)
//       dc2->SetDumpGdml(fDumpGdmlCmd->GetNewBoolValue(newValue));
//     if(command == fDumpGdmlFileNameCmd)
//       dc2->SetDumpGdmlFile(newValue);
//   }
}

