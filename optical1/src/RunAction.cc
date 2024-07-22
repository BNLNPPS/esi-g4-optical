#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "Run.hh"
#include "G4ParticleDefinition.hh"
#include "G4Run.hh"

// ---
RunAction::RunAction(PrimaryGeneratorAction* prim)
  : G4UserRunAction()
  , fRun(nullptr)
  , fPrimary(prim)
{}

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

