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
std::string runnumber = std::to_string( int(1000000*(1+G4UniformRand())) );

if(_OutPutFormat == "root"){
    fileName = "Run" + runnumber + ".root"; //select root format
    }
else if(_OutPutFormat == "csv")
{fileName = "Run" + runnumber + ".csv";}
else if(_OutPutFormat == "hdf5")
{fileName = "Run" + runnumber + ".hdf5";}



analysisManager->OpenFile(fileName);
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

  auto analysisManager = G4AnalysisManager::Instance();
  // Write and close file
  analysisManager->Write();
  analysisManager->CloseFile();
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
  if(fPrimary)
  {
    G4ParticleDefinition* particle =
      fPrimary->GetParticleGun()->GetParticleDefinition();
    G4double energy = fPrimary->GetParticleGun()->GetParticleEnergy();
    fRun->SetPrimary(particle, energy);
  }
}

// ---
void RunAction::EndOfRunAction(const G4Run*)
{
  if(isMaster)
    fRun->EndOfRun();
}