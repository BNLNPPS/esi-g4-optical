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

// #include "G4VisExecutive.hh"

#include "Randomize.hh"


// important for optical
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "FTFP_BERT.hh"
// ---
#include "lyra.hpp" // CLI options handling
#include <julia.h>

bool Steering::analysis, Steering::callback, Steering::verbose;

using namespace std;

JULIA_DEFINE_FAST_TLS // only define this once, in an executable

// -----------------------------------------------------------

int main(int argc,char** argv) {

  // We use lyra to parse the command line:
  bool help=false, batch=false, analysis=false, callback=false, verbose=false;
  G4String macro="", output_file="";
  string htitle="My Histogram", fn="image.png";
  int threads = 0, nevents = 100;

  float low=0.0, bin=0.5, high=100.0;

  auto cli = lyra::cli() 
    | lyra::opt(output_file, "output_file") ["-o"]["--output_file"]   ("Output file, default empty").optional()
    | lyra::opt(fn,          "fn")          ["-i"]["--image_file"]    ("Graphics Output file, default image.png").optional()    
    | lyra::opt(macro,       "macro")       ["-m"]["--macro"]         ("Optional macro").optional()
    | lyra::opt(htitle,      "title")       ["-T"]["--histogram_title"]("Histogram Title").optional()    
    | lyra::opt(nevents,     "nevents")     ["-n"]["--nevents"]       ("Optional: number of events").optional()
    | lyra::opt(threads,     "threads")     ["-t"]["--threads"]       ("Optional: number of threads").optional()
    | lyra::opt(low,         "low")         ["-L"]["--low"]           ("Optional: low limit for the histogram, default 0.").optional()
    | lyra::opt(high,        "high")        ["-H"]["--high"]          ("Optional: high limit for the histogram, default 100.").optional()
    | lyra::opt(bin,         "bin")         ["-B"]["--bin"]           ("Optional: bin width the histogram, default 0.5").optional()
    | lyra::opt(batch)                      ["-b"]["--batch"]         ("Optional batch mode").optional()
    | lyra::opt(callback)                   ["-j"]["--julia"]         ("Optional: Julia callback mode").optional()
    | lyra::opt(analysis)                   ["-a"]["--analysis"]      ("Optional: analysis mode").optional()
    | lyra::opt(verbose)                    ["-v"]["--verbose"]       ("Optional: verbose mode").optional()
    | lyra::opt(help)                       ["-h"]["--help"]          ("Help").optional();

  auto result = cli.parse({ argc, argv });

  if (!result) {std::cerr << "Error in command line, use -h for more info" << result.message() << std::endl; return 1;}
  // Optionally, print help and exit:
  if(help) {std::cout << cli << std::endl; exit(0);}


  // ----------------------------------------------------------------------------------------------------------------

  G4String  session;
  G4bool    verboseBestUnits = true;
  G4int     nThreads  = threads;


  Steering::analysis  = analysis;
  Steering::callback  = callback;
  Steering::verbose   = verbose;

  G4cout << "Number of Events:"       << nevents << G4endl;

  G4cout << "Steering::analysis:"     << Steering::analysis << G4endl;
  G4cout << "batch mode:"             << batch              << G4endl;
  G4cout << "Steering::callback:"     << Steering::callback << G4endl;
  G4cout << "Steering::verbose:"      << Steering::verbose  << G4endl;
  G4cout << "threads:"                << nThreads           << G4endl;
  G4cout << "histogram parameters:"   << low << ' ' << bin << ' ' << high  << G4endl;

  
  if(output_file.size())  {G4cout << "output file:"   << output_file  << G4endl;} else {G4cout << "output file not specified" << G4endl;}
  if(macro.size())        {G4cout << "macro file:"    << macro        << G4endl;} else {G4cout << "macro file not specified"  << G4endl;}

 

  int8_t st=0;
   // ####################### START JULIA SETUP ##########################
  if (Steering::callback) {

    G4cout << "MAIN -- initiating Julia callback" << G4endl;

    jl_init(); /* required: setup the Julia context */

    st = jl_gc_safe_enter(jl_current_task->ptls); // Handle the Julia GC mechanics, before execution:

    // if (jl_exception_occurred()) {
    //   G4cout << "Julia exception: " << jl_typeof_str(jl_exception_occurred()) << G4endl;
    //   jl_atexit_hook(0); // exit Julia
    //   exit(-1);
    // }


    jl_eval_string("include(\"./julia/my_module.jl\")");
    if (jl_exception_occurred()) {
      G4cout << "Julia exception when loading: " << jl_typeof_str(jl_exception_occurred()) << G4endl;
      jl_atexit_hook(0); // exit Julia
      exit(-1);
    }
      
    jl_eval_string("using .custom");

    // jl_eval_string("plot(rand(1000))");

    G4cout << "MAIN -- check if Julia is alive, expecting sqrt(2): " << G4endl;
    jl_eval_string("println(sqrt(2.0))"); // check Julia is alive

    jl_function_t* test_jl = jl_get_function(jl_main_module, "test_func");

    if (test_jl == NULL) {
      G4cout << "MAIN --  test_jl is null, exiting..." << G4endl;      
      jl_atexit_hook(0);
      exit(0);
    }
    else {
      G4cout << "MAIN -- test load of a Julia function test_func successful" << G4endl;
    }

    G4cout << "MAIN -- Calling test_func " << G4endl;

    jl_value_t *tf = jl_call0(test_jl); // just a "ping"
    int tF = jl_unbox_int8(tf);
    G4cout << "MAIN -- return from test_func... " <<  tF << G4endl;

    jl_value_t *jl_nt = jl_box_int8(nThreads);
    G4cout << "MAIN -- getting to the steering..." << G4endl;  

    jl_value_t *jl_low = jl_box_float64(low);
    jl_value_t *jl_bin = jl_box_float64(bin);
    jl_value_t *jl_high = jl_box_float64(high);    

    jl_function_t *jl_nthreads  = jl_get_function(jl_main_module, "nthreads");
    jl_function_t *jl_title     = jl_get_function(jl_main_module, "title");
    jl_function_t *jl_set_title = jl_get_function(jl_main_module, "set_title");
    jl_function_t *jl_set_fn    = jl_get_function(jl_main_module, "set_fn");

    jl_function_t *jl_set_hist  = jl_get_function(jl_main_module, "set_hist");

    jl_call3(jl_set_hist, jl_low, jl_bin, jl_high);

    jl_value_t *testn0 = jl_call0(jl_nthreads);
    int testN0 = jl_unbox_int8(testn0);
    G4cout << "MAIN -- default threads from Julia, N: " <<  testN0 << G4endl;

    jl_value_t *title = jl_call0(jl_title);

    const char *unboxed = jl_string_ptr(title); 
    G4cout << "MAIN -- default histogram title from Julia: " << unboxed << G4endl;

    // char const *julia = "julia";

    // string julia = title;

    jl_value_t *jtitle  = jl_cstr_to_string(htitle.c_str());
    jl_value_t *jfn     = jl_cstr_to_string(fn.c_str());

    // jl_value_t *argument = jl_box_char(*julia);
    jl_call1(jl_set_title,  jtitle);
    jl_call1(jl_set_fn,     jfn);

    title = jl_call0(jl_title);

    unboxed = jl_string_ptr(title); 
    G4cout << "MAIN -- updated histogram title from Julia: " << unboxed << G4endl;


    jl_function_t *jl_set_nthreads = jl_get_function(jl_main_module, "set_nthreads");

    jl_call1(jl_set_nthreads, jl_nt);
    jl_value_t *testn = jl_call0(jl_nthreads);
    int testN = jl_unbox_int8(testn);
    G4cout << "MAIN -- updated threads from Julia, N: " <<  testN << G4endl;  

    jl_function_t *begin_event_action_jl = jl_get_function(jl_main_module, "begin_event");
    if (begin_event_action_jl == NULL) {
      G4cout << "MAIN --  begin_event_action_jl is null, exiting..." << G4endl;
      jl_atexit_hook(0);
      exit(0);
    }
  }  // --- if callback

  // exit(0);

  // ####################### END JULIA SETUP ##########################

  // G4UIExecutive* ui = nullptr;
  // if (!batch) {
  //   argc = 1; // -- hacky but the Executive works with argv...
  //   ui = new G4UIExecutive(argc, argv); // ui = new G4UIExecutive(argc, argv, session);
  // }



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


  // original -- auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);


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
  // auto visManager = new G4VisExecutive("Quiet");
  // visManager->Initialize();

  // Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();



  // ################################################################################
  // Process macro, run the default sequence or start UI session
  
  if (batch) {
    if(macro.size()) {
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
      G4String beam = "/run/beamOn ";
      
      UImanager->ApplyCommand( beam + std::to_string(nevents));
    }
  }
  else  { // interactive mode : define UI session. NB breaks Julia...
    // G4cout << "************************ UI mode" << G4endl;
    // UImanager->ApplyCommand("/control/execute init_vis.mac");
    // if (ui->IsGUI()) {UImanager->ApplyCommand("/control/execute gui.mac");}
    // ui->SessionStart();
    // delete ui;
  }


  // Cleanup. User actions, the physics list and the detector description are owned and
  // deleted by the run manager, so don't delete them here
  
  if (Steering::callback) { // Handle the Julia GC mechanics, on exit
    jl_gc_safe_leave(jl_current_task->ptls, st); 
  }

  // delete visManager; -- commented out since the visManager breaks Julia
  delete runManager;

  if (Steering::callback) {
    jl_atexit_hook(0); // exit Julia
  }

}

// exit(0);

  // ### - test section begin

  // jl_init(); /* required: setup the Julia context */
  // int8_t st0 = jl_gc_safe_enter(jl_current_task->ptls); // Handle the Julia GC mechanics, before execution:
  // jl_eval_string("import Plots"); // test

  // jl_eval_string("include(\"./julia/my_module.jl\")");
  // if (jl_exception_occurred()) {G4cout << "Julia exception: " << jl_typeof_str(jl_exception_occurred()) << G4endl; jl_atexit_hook(0); exit(-1);}

  // jl_eval_string("using .custom");

  // G4cout << "MAIN -- check if Julia is alive, expecting sqrt(2): " << G4endl;
  // jl_eval_string("println(sqrt(2.0))"); // check Julia is alive


  // if (jl_exception_occurred()) {G4cout << "Julia exception: " << jl_typeof_str(jl_exception_occurred()) << G4endl; jl_atexit_hook(0); exit(-1);}

  // jl_function_t* test_jl1 = jl_get_function(jl_main_module, "test_func");

  // if (test_jl1 == NULL) {G4cout << "MAIN --  test_jl1 is null, exiting..." << G4endl; jl_atexit_hook(0); exit(0);}
  // else {G4cout << "MAIN -- test load of Julia successful" << G4endl;}


  // G4cout << "MAIN -- Calling test_func " << G4endl;

  // jl_value_t *tf1 = jl_call0(test_jl1);
  // int tF1 = jl_unbox_int8(tf1);
  // G4cout << "MAIN -- return from test_func... " <<  tF1 << G4endl;

  // jl_value_t *jl_nt1 = jl_box_int8(4);
  // G4cout << "MAIN -- getting to the steering..." << G4endl;  
  // jl_function_t *jl_nthreads1 = jl_get_function(jl_main_module, "nthreads");
  // jl_function_t *jl_set_nthreads1 = jl_get_function(jl_main_module, "set_nthreads");

  // jl_call1(jl_set_nthreads1, jl_nt1);
  // jl_value_t *testn1 = jl_call0(jl_nthreads1);
  // int testN1 = jl_unbox_int8(testn1);
  // G4cout << "MAIN -- getting threads from Julia, N: " <<  testN1 << G4endl;  

  // exit(0);

  // ### - test section end


// #############################################################################


    // ---
    // This was in the first version of this code:
    //    jl_eval_string("Base.include(Main, \"./julia/steering.jl\")");
    //    jl_eval_string("using .steering"); // load the user's module
    // ---

    // ---
    // This was in the first version of this code:
    // The actual payload Julia code is loaded here
    // jl_eval_string("Base.include(Main, \"./julia/custom_module.jl\")");
    // jl_eval_string("using .custom"); // load the user's module
    // ---

    // The updated way to include, now the module is retired anyhow
    // jl_eval_string("include(\"./julia/steering.jl\")");


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