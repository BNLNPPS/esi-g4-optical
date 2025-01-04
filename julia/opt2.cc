// ---

#include "Steering.hh"

// ---
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "WorkerInitialization.hh"


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


bool Steering::analysis;
bool Steering::callback;
bool Steering::verbose;

JULIA_DEFINE_FAST_TLS // only define this once, in an executable



// -----------------------------------------------------------

int main(int argc,char** argv) {
  /* required: setup the Julia context */
  jl_init();

  /* run Julia commands */
  //  jl_eval_string("println(sqrt(2.0))"); // check Julia is alive

  jl_eval_string("Base.include(Main, \"./julia/custom_module.jl\")"); // load the user's module
  jl_eval_string("using .custom");


  jl_function_t *foo= jl_get_function(jl_main_module, "Foo");
  jl_function_t *opstruct= jl_get_function(jl_main_module, "opstruct");
// if (test_func != NULL) {printf("test_func is not null\n");}
// else {printf("test_func is null\n"); jl_atexit_hook(0); return 0;}
// jl_call0(test_func);


  // --mxp--: We use lyra to parse the command line:
  bool help       = false;
  bool batch      = false;
  bool analysis   = false;
  bool callback   = false;
  bool verbose    = false;

  G4String macro          = "";
  G4String output_file    = "";

  int threads = 0;

  auto cli = lyra::cli() 
    | lyra::opt(output_file, "output_file")
    ["-o"]["--output_file"]
    ("Output file, default empty").optional()
    | lyra::opt(macro, "macro")
    ["-m"]["--macro"]
    ("Optional macro").optional()
    | lyra::opt(threads, "threads")
    ["-t"]["--threads"]
    ("Optional number of threads").optional()
    | lyra::opt(batch)
    ["-b"]["--batch"]
    ("Optional batch mode").optional()
    | lyra::opt(callback)    
    ["-c"]["--callback"]
    ("Optional callback mode").optional()
    | lyra::opt(analysis)
    ["-a"]["--analysis"]
    ("Optional analysis mode").optional()
    | lyra::opt(verbose)
    ["-v"]["--verbose"]    
    ("Verbose").optional()
    | lyra::opt(help)
    ["-h"]["--help"]
    ("Help").optional();

  auto result = cli.parse({ argc, argv });

  if (!result) {std::cerr << "Error in command line, use -h for more info" << result.message() << std::endl; return 1;}
  // Optionally, print help and exit:
  if(help) {std::cout << cli << std::endl; exit(0);}

  // exit(0);

  G4String  session;
  G4bool    verboseBestUnits = true;
  G4int     nThreads  = threads;


  Steering::analysis  = analysis;
  Steering::callback  = callback;
  Steering::verbose   = verbose;

  G4cout << "Steering::analysis:"     << Steering::analysis << G4endl;
  G4cout << "batch mode:"             << batch              << G4endl;
  G4cout << "Steering::callback:"     << Steering::callback << G4endl;
  G4cout << "Steering::verbose:"      << Steering::verbose  << G4endl;
  G4cout << "threads:"                << nThreads           << G4endl;

  
  if(output_file.size())  {G4cout << "output file:"   << output_file  << G4endl;} else {G4cout << "output file not specified" << G4endl;}
  if(macro.size())        {G4cout << "macro file:"    << macro        << G4endl;} else {G4cout << "macro file not specified"  << G4endl;}

  // -- hacky but the Executive works with argv...
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

  // ------------------ RUN MANAGER -----------------------------------
  G4RunManager* runManager;



  if (nThreads > 0) { 
    runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);
    runManager->SetNumberOfThreads(nThreads);
    runManager->SetVerboseLevel(0);
    runManager->SetUserInitialization(new WorkerInitialization());
  } else {
    runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);
  }

  // auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);


  // Set mandatory initialization classes
  //
  auto detConstruction = new DetectorConstruction();
  runManager->SetUserInitialization(detConstruction);

  auto physicsList = new FTFP_BERT(0);

  // set up optics -- note that "0" in the following line really helps to reduce output,
  // which is otherwise pretty massive.
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option4(0));
  auto opticalPhysics = new G4OpticalPhysics();
  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);

  auto actionInitialization = new ActionInitialization(detConstruction, output_file);
  runManager->SetUserInitialization(actionInitialization);

  // Initialize visualization and make it quiet... see /vis/verbose guidance.
  auto visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();

  auto state = jl_gc_safe_enter(jl_current_task->ptls);

  // ################################################################################
  // Process macro or start UI session
  //
  
  if (batch) {
    if(macro.size()) {
      // batch mode
      G4cout << "************************ Batch execution of the macro:"        << macro        << G4endl;
      G4String command = "/control/execute ";
      UImanager->ApplyCommand(command+macro);
    }
    else {
      G4cout << "************************ Default Batch execution"  << G4endl;
      UImanager->ApplyCommand("/run/initialize");
      UImanager->ApplyCommand("/control/verbose 0");
      UImanager->ApplyCommand("/tracking/verbose 0");
      UImanager->ApplyCommand("/gun/particle mu+");
      UImanager->ApplyCommand("/gun/energy 300 MeV");
      UImanager->ApplyCommand("/run/beamOn 1000");
    }
  }
  else  { // interactive mode : define UI session
    G4cout << "************************ UI mode" << G4endl;
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) {UImanager->ApplyCommand("/control/execute gui.mac");}
    ui->SessionStart();
    delete ui;
  }

  // Cleanup. User actions, the physics list and the detector description are owned and
  // deleted by the run manager, so don't delete them here

  jl_gc_safe_leave(jl_current_task->ptls, state);

  delete visManager;
  delete runManager;

  jl_atexit_hook(0); // exit Julia

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

//<< result.message() << std::endl;


// -- Just park this code here for reference
// jl_function_t *test_func= jl_get_function(jl_main_module, "test_func");
// if (test_func != NULL) {printf("test_func is not null\n");}
// else {printf("test_func is null\n"); jl_atexit_hook(0); return 0;}
// jl_call0(test_func);