#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "Run.hh"
#include "G4ParticleDefinition.hh"
#include "G4Run.hh"
#include <string>
#include <fstream>
#include "Randomize.hh"

#include "G4AnalysisManager.hh"

// ---
RunAction::RunAction(PrimaryGeneratorAction* prim, std::string OutPutFormat)
  : G4UserRunAction()
  , fRun(nullptr)
  , fPrimary(prim)
{
_OutPutFormat = OutPutFormat;

auto analysisManager = G4AnalysisManager::Instance(); //get the manager
analysisManager->SetNtupleMerging(true);
analysisManager->CreateNtuple("Ntuple", "Ntuple");
analysisManager->CreateNtupleDColumn("Energy");
analysisManager->CreateNtupleDColumn("X");
analysisManager->CreateNtupleDColumn("Y");
analysisManager->CreateNtupleDColumn("Z");
analysisManager->CreateNtupleDColumn("Dir_X");
analysisManager->CreateNtupleDColumn("Dir_Y");
analysisManager->CreateNtupleDColumn("Dir_Z");
analysisManager->FinishNtuple();
}


RunAction::~RunAction(){
}

// ---
G4Run* RunAction::GenerateRun()
{
  fRun = new Run();
  return fRun;
}

// ---
void RunAction::BeginOfRunAction(const G4Run*)
{


  auto analysisManager = G4AnalysisManager::Instance(); //get the manager

  if      (_OutPutFormat == "root")  { fileName = "Run.root";  }
  else if (_OutPutFormat == "csv")   { fileName = "Run.csv";   }
  else if (_OutPutFormat == "hdf5")  { fileName = "Run.hdf5";  }

  analysisManager->OpenFile(fileName);
  
  
  if(fPrimary) {
    G4ParticleDefinition* particle  = fPrimary->GetParticleGun()->GetParticleDefinition();
    G4double energy                 = fPrimary->GetParticleGun()->GetParticleEnergy();
    
    fRun->SetPrimary(particle, energy);
  }
}

// ---
void RunAction::EndOfRunAction(const G4Run*) {

  auto analysisManager = G4AnalysisManager::Instance();
  if(isMaster) {fRun->EndOfRun();}

  // Write and close file
  analysisManager->Write();
  analysisManager->CloseFile();
  

}