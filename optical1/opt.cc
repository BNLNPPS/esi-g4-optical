#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"

#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"


// CLI aruments handling:
#include "lyra.hpp"


int main(int argc, char **argv)
{

  bool verbose        =   false;
  bool cimode         =   false;
  bool help           =   false;
  std::string mac_name = "vis.mac";
  size_t taps_start   = 4;
   
  auto cli = lyra::cli()
      | lyra::opt(verbose)
        ["-v"]["--verbose"]("bool - verbose mode" )
      | lyra::help(help)
      | lyra::opt(mac_name, "mac_name")
        ["-m"]["--mac_name"]("taps_start: default 4, starting point for tap scan")
    ;

  auto result = cli.parse({ argc, argv });


  // Detect interactive mode (if no argument) and define UI session
  G4UIExecutive *ui = 0;
  if (argc == 1)
  { // No commands line argument, G4UIExecutive will guess what is the best available UI
    ui = new G4UIExecutive(argc, argv);
  }

  // Construct the default run manager
  auto *runManager = G4RunManagerFactory::CreateRunManager();
  runManager->SetUserInitialization(new DetectorConstruction);
  runManager->SetVerboseLevel(0);

  // The Physics --
  //  Cf L158 extended/opticall/OpNovice.cc 11.2.1
  G4VModularPhysicsList *physicsList = new FTFP_BERT(0);

  // Original
  // physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  // runManager->SetUserInitialization(physicsList);
  // Replace with Optics

  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());
  auto opticalPhysics = new G4OpticalPhysics();
  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new ActionInitialization());

  // Visualization manager construction
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.

  G4VisManager *visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();


  // Around L166 in OpNovicw
  // Get the pointer to the User Interface manager
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  if (argc > 1)
  {
    // execute an argument macro file if exist
    G4String command = "/control/execute ";
    G4String fileName = mac_name;
    UImanager->ApplyCommand(command + fileName);
  }
  else
  {
    // We have visualization, initialize defaults: look in init_vis.mac macro
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI())
    {
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    ui->SessionStart();
    delete ui;
  }
  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted in the main() program !

  delete visManager;
  delete runManager;

  return 0;
}
