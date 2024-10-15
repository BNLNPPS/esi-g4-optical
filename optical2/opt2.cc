#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
// important for optical
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "FTFP_BERT.hh"
// ---

#include "Randomize.hh"

// CLI aruments handling:
#include "lyra.hpp"

// ---

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " Opt2 [-m macro ] [-u UIsession] [-t nThreads] [-vDefault]"
           << G4endl;
    G4cerr << "   note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}

// ---

int main(int argc,char** argv)
{
  // Evaluate arguments
  if ( argc > 7 ) {PrintUsage(); return 1;}

  // --mxp--: We use lyra to parse the command line:
  bool help               =   false;
  G4String macro          = "init_vis.mac";
  G4String output_format  = "";  
  int threads = 0;

  auto cli = lyra::cli()
      | lyra::help(help)
      | lyra::opt(macro, "macro")
        ["-m"]["--macro"]("macro: default: init_vis.mac, otherwise specify the name of the G4 macro file to be run")
      | lyra::opt(output_format, "output_format")
        ["-f"]["--output_format"]("default: '', options: csv, root, hdf5")
      | lyra::opt(threads, "threads")
        ["-T"]["--threads"]("Number of threads (default 0)")
    ;

  auto result = cli.parse({ argc, argv });

  // Optionally, print help and exit:
  if(help) {std::cout << cli << std::endl; exit(0);}


  G4String  session;
  G4bool    verboseBestUnits = true;

  G4int nThreads = 0;
#ifdef G4MULTITHREADED
  nThreads = threads;
#endif


  G4cout << "threads:" << nThreads << G4endl;
  G4cout << "output format:" << output_format << G4endl;
  G4cout << "macro:" << macro << G4endl;

  exit(0);

  // Detect interactive mode (if no macro provided) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( ! macro.size() ) {
    ui = new G4UIExecutive(argc, argv, session);
  }

  // Optionally: choose a different Random engine...
  // G4Random::setTheEngine(new CLHEP::MTwistEngine);

  // Use G4SteppingVerboseWithUnits
  if ( verboseBestUnits ) {
    G4int precision = 8;
    G4SteppingVerbose::UseBestUnit(precision);
  }

  // Construct the default run manager
  auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  runManager->SetVerboseLevel(0);
#ifdef G4MULTITHREADED
  if ( nThreads > 0 ) {
    runManager->SetNumberOfThreads(nThreads);
  }
#endif

  // Set mandatory initialization classes
  //
  auto detConstruction = new DetectorConstruction();
  runManager->SetUserInitialization(detConstruction);

  auto physicsList = new FTFP_BERT(0);

  // make optics works
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4());

  auto opticalPhysics = new G4OpticalPhysics();
  physicsList->RegisterPhysics(opticalPhysics);

  //

  runManager->SetUserInitialization(physicsList);

  auto actionInitialization = new ActionInitialization(detConstruction);
  runManager->SetUserInitialization(actionInitialization);

  // Initialize visualization and make it quiet...
  auto visManager = new G4VisExecutive("Quiet");
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // auto visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if ( macro.size() ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else  {
    // interactive mode : define UI session
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    ui->SessionStart();
    delete ui;
  }

  // Termination: free the store. User actions, physics_list and detector_description are owned and
  // deleted by the run manager, so they should not be deleted in the main() program

  delete visManager;
  delete runManager;
}

//   for ( G4int i=1; i<argc; i=i+2 ) {
//     if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
//     else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];

// #ifdef G4MULTITHREADED
//     else if ( G4String(argv[i]) == "-t" ) {

//       nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
//     }
// #endif
//     else if ( G4String(argv[i]) == "-vDefault" ) {
//       verboseBestUnits = false;
//       --i;  // this option is not followed with a parameter
//     }
//     else {
//       PrintUsage();
//       return 1;
//     }
//   }
