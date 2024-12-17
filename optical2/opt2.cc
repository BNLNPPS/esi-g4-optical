#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"
// important for optical
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "FTFP_BERT.hh"
// ---
// CLI aruments handling:
#include "lyra.hpp"

#include <julia.h>

JULIA_DEFINE_FAST_TLS

// -----------------------------------------------------------

int main(int argc,char** argv) {
  /* required: setup the Julia context */
  jl_init();

  /* run Julia commands */
  jl_eval_string("println(sqrt(2.0))");
  jl_eval_string("Base.include(Main, \"./julia/custom_module.jl\")");



  // --------------------------------------------------
  jl_eval_string("Base.include(Main, \"./custom_module.jl\")");
  jl_eval_string("using .custom");
  jl_function_t *func= jl_get_function(jl_main_module, "func");

  if (func != NULL) {
      printf("func is not null\n");
  }
  else {
      printf("func is null\n");
      jl_atexit_hook(0);
      return 0;
  }

  jl_call0(func);


  // --mxp--: We use lyra to parse the command line:
  bool help               =   false;
  bool batch              =   false;

  G4String macro          = "init_vis.mac";
  G4String output_file    = "";

  //  G4cout << output_file.length() << G4endl;
  // exit(0);

  int threads = 0;

  auto cli = lyra::cli();
  cli |= lyra::opt(output_file, "output_file")["-o"]["--output_file"]("Output file, default empty").optional();
  cli |= lyra::opt(macro,       "macro")["-m"]["--macro"]("Optional macro").optional();
  cli |= lyra::opt(batch,       "batch")["-b"]["--batch"]("Optional batch mode").optional();
  cli |= lyra::opt(threads,     "threads")["-t"]["--threads"]("Optional number of threads").optional();

  auto result = cli.parse({ argc, argv });
  if (!result) {
    std::cerr << "Error in command line, use -h for more info" << std::endl;
    return 1;
  }

  // Optionally, print help and exit:
  if(help) {std::cout << cli << std::endl; exit(0);}

  G4String  session;
  G4bool    verboseBestUnits = true;

  G4int nThreads = 0;
#ifdef G4MULTITHREADED
  nThreads = threads;
#endif

  G4cout << "batch mode:"   << batch        << G4endl;
  G4cout << "threads:"      << nThreads     << G4endl;
  G4cout << "output file:"  << output_file  << G4endl;
  G4cout << "macro:"        << macro        << G4endl;

  // exit(0);

  // Detect interactive mode (if no macro provided) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( ! batch ) {
    argc = 1;
    ui = new G4UIExecutive(argc, argv); // ui = new G4UIExecutive(argc, argv, session);
  }

  // Optionally: choose a different Random engine... G4Random::setTheEngine(new CLHEP::MTwistEngine);

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

  // ---
  runManager->SetUserInitialization(physicsList);

  auto actionInitialization = new ActionInitialization(detConstruction, output_file);
  runManager->SetUserInitialization(actionInitialization);

  // Initialize visualization and make it quiet... see /vis/verbose guidance.
  auto visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();


  // ################################################################################
  // Process macro or start UI session
  //
  if ( macro.size() && batch ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else  { // interactive mode : define UI session
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) {UImanager->ApplyCommand("/control/execute gui.mac");}
    ui->SessionStart();
    delete ui;
  }

  // Cleanup. User actions, the physics list and the detector description are owned and
  // deleted by the run manager, so don't delete them here

  delete visManager;
  delete runManager;

  jl_atexit_hook(0);

}

// #############################################################################
// The old CLI UI, kept for reference only
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
