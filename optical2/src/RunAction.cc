#include "RunAction.hh"
#include "RunData.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

// ---
// Define print frequency, analysis manager and its verbosity

RunAction::RunAction() {

  // set printing event number per each event... Mind gui.mac!
  G4RunManager::GetRunManager()->SetPrintProgress(10);

  // Create analysis manager.  The choice of analysis technology is done via selectin of a namespace in Analysis.hh
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  // analysisManager->SetHistoDirectoryName("histograms");
  // analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(2);
  analysisManager->SetNtupleMerging(true);

  analysisManager->CreateH1("Nphotons" ,"Nphotons", 100, 0., 50000.);

  // -mxp-
  return;
    // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple

  // analysisManager->CreateH1("Eabs" ,"Edep in absorber", 110, 0., 330*MeV);
  // analysisManager->CreateH1("Egap" ,"Edep in gap", 100, 0., 30*MeV);
  // analysisManager->CreateH1("Labs" ,"trackL in absorber", 100, 0., 50*cm);
  // analysisManager->CreateH1("Lgap" ,"trackL in gap", 100, 0., 50*cm);
  
  // // Creating ntuple
  // analysisManager->CreateNtuple("B4", "Edep and TrackL");
  // analysisManager->CreateNtupleDColumn("Eabs");
  // analysisManager->CreateNtupleDColumn("Egap");
  // analysisManager->CreateNtupleDColumn("Labs");
  // analysisManager->CreateNtupleDColumn("Lgap");
  // analysisManager->FinishNtuple();
}

// ---

G4Run* RunAction::GenerateRun() {
  return (new RunData);
}

// ---

void RunAction::BeginOfRunAction(const G4Run* run) {
  // -mxp-

  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
  G4cout << "### Filename ---" << _filename << "---" << G4endl;


  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  // G4String fileName = "B4.root";  // Other supported output types: csv, hdf5, xml
  analysisManager->OpenFile(_filename);
  G4cout << "##### Using " << analysisManager->GetType() << G4endl;
  G4cout << "##### FileName " << analysisManager->GetFileName() << G4endl;
  G4cout << "##### IsOpen "<< analysisManager->IsOpenFile() << G4endl;

  return;
}

// ---

// Note we print total number of the Cherenkov photons for now...

void RunAction::EndOfRunAction(const G4Run* /*aRun*/) {
  // --mxp--
  auto runData = static_cast<RunData*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  G4int N = runData->GetNphotons();
  G4cout << "Photons " << N << G4endl;

  auto analysisManager = G4AnalysisManager::Instance();

  G4cout << "Mean  " << analysisManager->GetH1(0)->mean() << G4endl;

  analysisManager->Write();
  analysisManager->CloseFile();
  return;


  // print histogram statistics
  // auto analysisManager = G4AnalysisManager::Instance();
  if ( analysisManager->GetH1(1) ) {
    G4cout << G4endl << " ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl;
    }
    else {
      G4cout << "for the local thread " << G4endl << G4endl;
    }

    G4cout << " EAbs : mean = "
       << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy")
       << " rms = "
       << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;

    G4cout << " EGap : mean = "
       << G4BestUnit(analysisManager->GetH1(1)->mean(), "Energy")
       << " rms = "
       << G4BestUnit(analysisManager->GetH1(1)->rms(),  "Energy") << G4endl;

    G4cout << " LAbs : mean = "
      << G4BestUnit(analysisManager->GetH1(2)->mean(), "Length")
      << " rms = "
      << G4BestUnit(analysisManager->GetH1(2)->rms(),  "Length") << G4endl;

    G4cout << " LGap : mean = "
      << G4BestUnit(analysisManager->GetH1(3)->mean(), "Length")
      << " rms = "
      << G4BestUnit(analysisManager->GetH1(3)->rms(),  "Length") << G4endl;
  }

  // save histograms & ntuple
  analysisManager->Write();
  analysisManager->CloseFile();

}

