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

  bool help           =   false;
  std::string mac_name = "init_vis.mac";
  std::string output_format = "";
   

  // We use lyra to parse the command line:
  auto cli = lyra::cli()
      | lyra::help(help)
      | lyra::opt(mac_name, "mac_name")
        ["-m"]["--mac_name"]("mac_name: default: init_vis.mac, otherwise specify the name of the G4 macro file to be run")
      | lyra::opt(output_format, "output_format")
        ["-f"]["--output_format"]("default: '', options: csv, root, hdf5")
    ;

  auto result = cli.parse({ argc, argv });

  // Optionally, print help and exit:
  if(help) {
    std::cout << cli << std::endl;
    exit(0);
  }


  // --------------------------------------------------------------
  // Detect interactive mode (if no argument) and define UI session
  G4UIExecutive *ui = 0;

  // Construct the default run manager
  auto *runManager = G4RunManagerFactory::CreateRunManager();
  runManager->SetUserInitialization(new DetectorConstruction);
  runManager->SetVerboseLevel(0);

  // The Physics... Cf. L158 extended/opticall/OpNovice.cc 11.2.1
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
  runManager->SetUserInitialization(new ActionInitialization(output_format));

  // Visualization manager construction
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.

  G4VisManager *visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  if(argc != 1 && mac_name == "init_vis.mac") 
      {
      G4cerr << "   *****************************************" <<  G4endl;
      G4cerr << "Macro file must be specified if the executable is not called without arguments!" <<  G4endl;
      G4cerr << "   *****************************************" <<  G4endl;
      exit(-1);
      }


  if(argc == 1) ui = new G4UIExecutive(argc, argv);

  // Get the pointer to the User Interface manager
  G4UImanager *UImanager = G4UImanager::GetUIpointer();
  G4String fileName = mac_name;
  G4String command = "/control/execute ";
  UImanager->ApplyCommand(command + fileName);
  
  if (argc == 1) 
    {
      UImanager->ApplyCommand("/control/execute gui.mac");
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
