#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "Run.hh"
#include "G4ParticleDefinition.hh"
#include "G4Run.hh"
#include <string>
#include <mutex>
#include <fstream>
std::mutex photonDataMutex;

// ---
RunAction::RunAction(PrimaryGeneratorAction* prim)
  : G4UserRunAction()
  , fRun(nullptr)
  , fPrimary(prim)
{printOutBufferSize = 1000;}





RunAction::~RunAction(){
    std::fstream CerenkovFile(std::string("CerenkovPhotons")+
		std::to_string(G4Threading::G4GetThreadId())+".dat", std::ios_base::app);
		for (const auto& event : PhotonData) {
        for (int k=1; k<event.size(); ++k) 
              CerenkovFile << " " << event.at(k);
			CerenkovFile << G4endl;
		}
    PhotonData.clear();
		CerenkovFile.close();
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
void RunAction::AddPhoton(std::vector<double> Photon){

 
  //std::lock_guard<std::mutex> lock(photonDataMutex);
 PhotonData.push_back(Photon);

 if (PhotonData.size() == printOutBufferSize){

    std::fstream CerenkovFile(std::string("CerenkovPhotons")+
		std::to_string(G4Threading::G4GetThreadId())+".dat", std::ios_base::app);
		for (const auto& event : PhotonData) {
        for (int k=1; k<event.size(); ++k) 
              CerenkovFile << " " << event.at(k);
			CerenkovFile << G4endl;
		}
    PhotonData.clear();
		CerenkovFile.close();

    }
 
}
