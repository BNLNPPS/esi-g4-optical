#include "RunAction.hh"
#include "RunData.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

namespace B4b
{

// ---

RunAction::RunAction() {
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // Create analysis manager.  The choice of analysis technology is done via selectin of a namespace in Analysis.hh
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  //analysisManager->SetHistoDirectoryName("histograms");
  //analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
    // Note: merging ntuples is available only with Root output

  // Book histograms, ntuple

  analysisManager->CreateH1("Eabs" ,"Edep in absorber", 110, 0., 330*MeV);
  analysisManager->CreateH1("Egap" ,"Edep in gap", 100, 0., 30*MeV);
  analysisManager->CreateH1("Labs" ,"trackL in absorber", 100, 0., 50*cm);
  analysisManager->CreateH1("Lgap" ,"trackL in gap", 100, 0., 50*cm);
  
  // Creating ntuple
  analysisManager->CreateNtuple("B4", "Edep and TrackL");
  analysisManager->CreateNtupleDColumn("Eabs");
  analysisManager->CreateNtupleDColumn("Egap");
  analysisManager->CreateNtupleDColumn("Labs");
  analysisManager->CreateNtupleDColumn("Lgap");
  analysisManager->FinishNtuple();
}

// ---

G4Run* RunAction::GenerateRun() {
  return (new RunData);
}

// ---

void RunAction::BeginOfRunAction(const G4Run* run) {
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;

  //inform the runManager to save random number seed
  //G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "B4.root";
  // Other supported output types:
  // G4String fileName = "B4.csv";
  // G4String fileName = "B4.hdf5";
  // G4String fileName = "B4.xml";
  analysisManager->OpenFile(fileName);
  G4cout << "Using " << analysisManager->GetType() << G4endl;
}

// ---

void RunAction::EndOfRunAction(const G4Run* /*aRun*/) {
  // print histogram statistics
  auto analysisManager = G4AnalysisManager::Instance();
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

}
