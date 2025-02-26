// ---

#include "Steering.hh"

#include "RunAction.hh"
#include "RunData.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <chrono>
using namespace std::chrono;

std::time_t start, stop;

// ---
// Define print frequency, analysis manager and its verbosity

RunAction::RunAction(const G4String fn, bool master) {
  _filename = fn;
  _master   = master;

  if (Steering::callback) {
    begin_run_jl = jl_get_function(jl_main_module, "begin_run");
    if (begin_run_jl == NULL) {
      G4cout << "Run Action ctor --  begin_run_jl is null." << G4endl;
      // jl_atexit_hook(0);
      // exit(0);
    }

    end_run_jl = jl_get_function(jl_main_module, "end_run");
    if (begin_run_jl == NULL) {
      G4cout << "Run Action ctor --  end_run_jl is null." << G4endl;
      // jl_atexit_hook(0);
      // exit(0);
    }
  }

  G4RunManager::GetRunManager()->SetPrintProgress(-1);  // set printing event number per each event... Mind gui.mac!

  if (_filename.length()==0) { return;}

  // Create analysis manager.  The choice of analysis technology is done via selectin of a namespace in Analysis.hh
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  // analysisManager->SetHistoDirectoryName("histograms");
  // analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(2);
  analysisManager->SetNtupleMerging(true);

  // Book histograms, ntuple
  analysisManager->CreateH1("Nphotons" ,"Nphotons", 100, 0., 50000.);

  // Creating ntuple
  analysisManager->CreateNtuple("Opt2", "Direction");
  analysisManager->CreateNtupleDColumn("Xdir");
  analysisManager->CreateNtupleDColumn("Ydir");
  analysisManager->CreateNtupleDColumn("Zdir");
  analysisManager->CreateNtupleDColumn("Energy");
  analysisManager->FinishNtuple();
  return;

}

// ---

G4Run* RunAction::GenerateRun() {
  return (new RunData());
}

// ---

void RunAction::BeginOfRunAction(const G4Run* run) {
  auto id = G4Threading::G4GetThreadId();
  G4cout << "=====> RUN START REPORT <===========   id: " << id << G4endl;

  if(_master) {
    G4cout << "=====> RUN START MASTER" << G4endl;
    if(begin_run_jl) {
      jl_call0(begin_run_jl);
    }
  }

  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
  if (_filename.length()==0) {
    start = system_clock::to_time_t(system_clock::now());
    std::cout << "##### Time start = " << std::ctime(&start) << " \n";
    return;
  }

  G4cout << "### Filename ---" << _filename << "---" << G4endl;


  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  if (_filename.length()>0) {
    analysisManager->OpenFile(_filename);
    G4cout << "##### Using "    << analysisManager->GetType()     << G4endl;
    G4cout << "##### FileName " << analysisManager->GetFileName() << G4endl;
    G4cout << "##### IsOpen "   << analysisManager->IsOpenFile()  << G4endl;
  }
  else {
    G4cout << "##### RunAction: no FILENAME specifed for the Analysis Manager " << G4endl;
  }

  std::time_t start = system_clock::to_time_t(system_clock::now());
  std::cout << "Time start = " << std::ctime(&start) << " \n";

  return;
}

// ---
void RunAction::EndOfRunAction(const G4Run* /*aRun*/) {

  if(_master) {
    G4cout << "=====> RUN END MASTER" << G4endl;
    if(end_run_jl) {
      jl_call0(end_run_jl);
    }
  }


  if (_filename.length()==0) {

    stop = system_clock::to_time_t(system_clock::now());
    G4cout << "##### Time stop = " << std::ctime(&stop) << G4endl;
    //nanoseconds diff = stop - start;
    double diff = difftime(stop, start);
    // G4cout << diff << G4endl;
    // G4cout << "+++++++++++++++++++++++++++++++ END OF RUN ++++++++++++++++++++++++++++++++++++++++++++++" << G4endl;
    return;
  }
  // G4cout << "+++++++++++++++++++++++++++++++ END OF RUN ++++++++++++++++++++++++++++++++++++++++++++++" << G4endl;

  auto runData = static_cast<RunData*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  G4int N = runData->GetNphotons();
  G4cout << "Photons *********************************************** " << N << G4endl;

  auto analysisManager = G4AnalysisManager::Instance();

  G4cout << "Mean  ******************************************** " << analysisManager->GetH1(0)->mean() << G4endl;

  analysisManager->Write();
  analysisManager->CloseFile();
  return;

}

