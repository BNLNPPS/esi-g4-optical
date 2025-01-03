// ---

#include "Steering.hh"

// ---
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIcommand.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"

// Keep here temporarily
#include "G4UserWorkerInitialization.hh"

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


class WorkerInitialization : public G4UserWorkerInitialization {
  public:
    WorkerInitialization() = default;
    virtual ~WorkerInitialization() = default;
    virtual void WorkerInitialize() const override {
      if (jl_get_pgcstack() == NULL) {
      jl_adopt_thread();
      std::cout << "=====> WORKER INIT, THREAD ADOPTED <===========" << std::endl;
      }
    }
    virtual void WorkerStart() const override {}
    virtual void WorkerRunStart() const override {
      std::cout << "=====> WORKER RUN START <===========" << std::endl;
      //jl_eval_string("println(sqrt(2.0))");
      jl_function_t *test_func = jl_get_function(jl_main_module, "test_func");
      if (test_func == NULL) {
        G4cout << "Worker run start --  test_func is null, exiting..." << G4endl;      
        jl_atexit_hook(0);
        exit(0);
      }
    G4cout << "Calling test_func " << G4endl;
    jl_call0(test_func);


    jl_value_t *argument = jl_box_float64(2.0);
    jl_function_t *operation = jl_get_function(jl_main_module, "operation");

    jl_value_t *op_ret = jl_call1(operation, argument);

    if (jl_typeis(op_ret, jl_float64_type)) {
      double ret_unboxed = jl_unbox_float64(op_ret);
      G4cout << "WORKER RUN START - op_ret in C: " <<  ret_unboxed << G4endl;
    }
    else {
      G4cout << "ERROR: unexpected return type from op_ret" << G4endl;
    }    
  
    
    }

    virtual void WorkerRunEnd() const override {
    jl_value_t *argument = jl_box_float64(3.0);
    jl_function_t *operation = jl_get_function(jl_main_module, "operation");

    jl_value_t *op_ret = jl_call1(operation, argument);

    if (jl_typeis(op_ret, jl_float64_type)) {
      double ret_unboxed = jl_unbox_float64(op_ret);
      G4cout << "Worker RUN END op_ret in C: " <<  ret_unboxed << G4endl;
    }
    else {
      G4cout << "ERROR: unexpected return type from op_ret" << G4endl;
    }   


      jl_ptls_t ptls = jl_current_task->ptls;
      jl_gc_safe_enter(ptls);
    }
    virtual void WorkerStop() const override {}
};

// -----------------------------------------------------------

int main(int argc,char** argv) {
  /* required: setup the Julia context */
  jl_init();

  /* run Julia commands */
  //  jl_eval_string("println(sqrt(2.0))"); // check Julia is alive

  jl_eval_string("Base.include(Main, \"./julia/custom_module.jl\")"); // load the user's module
  jl_eval_string("using .custom");

  // --mxp--: We use lyra to parse the command line:
  bool help       = false;
  bool batch      = false;
  bool analysis   = false;
  bool callback   = false;
  bool verbose    = false;

  G4String macro          = "init_vis.mac";
  std::string output_file    = "";

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

  
  if(output_file.length()>0) {
    G4cout << "output file:"  << output_file  << G4endl;
  }
  else {
    G4cout << "output file not specified" << G4endl;
  }
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

  // ------------------ RUN MANAGER -----------------------------------
  G4RunManager* runManager;

  if (nThreads > 0) { 
    runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT);
    runManager->SetNumberOfThreads(nThreads);
    runManager->SetUserInitialization(new WorkerInitialization());
  } else {
    runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);
  }

  // auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  runManager->SetVerboseLevel(0);
// #ifdef G4MULTITHREADED
  // if ( nThreads > 0 ) {
  //   runManager->SetNumberOfThreads(nThreads);
  // }
//#endif

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