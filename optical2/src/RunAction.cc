#include "RunAction.hh"
#include "RunData.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

// ---
// Define print frequency, analysis manager and its verbosity

RunAction::RunAction(const G4String fn) {
  _filename=fn;

  G4RunManager::GetRunManager()->SetPrintProgress(10);  // set printing event number per each event... Mind gui.mac!

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
  // -mxp-

  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
  if (_filename.length()==0) { return;}

  G4cout << "### Filename ---" << _filename << "---" << G4endl;


  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  if (_filename.length()>0) {
    analysisManager->OpenFile(_filename);
    G4cout << "##### Using " << analysisManager->GetType() << G4endl;
    G4cout << "##### FileName " << analysisManager->GetFileName() << G4endl;
    G4cout << "##### IsOpen "<< analysisManager->IsOpenFile() << G4endl;
  }
  else {
    G4cout << "##### RunAction: no FILENAME specifed for the Analysis Manager " << G4endl;
  }

  return;
}

// ---

// Note we print total number of the Cherenkov photons for now...

void RunAction::EndOfRunAction(const G4Run* /*aRun*/) {
  //

  if (_filename.length()==0) {return;}


  G4cout << "+++++++++++++++++++++++++++++++ END OF RUN ++++++++++++++++++++++++++++++++++++++++++++++";

  auto runData = static_cast<RunData*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  G4int N = runData->GetNphotons();
  G4cout << "Photons *********************************************** " << N << G4endl;

  auto analysisManager = G4AnalysisManager::Instance();

  G4cout << "Mean  ******************************************** " << analysisManager->GetH1(0)->mean() << G4endl;

  analysisManager->Write();
  analysisManager->CloseFile();
  return;

}

