#ifndef Run_h
#define Run_h 1

#include "G4Run.hh"

class G4ParticleDefinition;

// ---

class Run : public G4Run
{
 public:
  Run() = default;
  ~Run() override = default;

  void SetPrimary(G4ParticleDefinition* particle, G4double energy);

  void AddCerenkov(G4double n)
  {
    fCerenkovCounter += n;
    fCerenkov2 += n * n;
  };
  void AddScintillation(G4double n)
  {
    fScintillationCounter += n;
    fScintillation2 += n * n;
  };
  void AddRayleigh(G4double n)
  {
    fRayleighCounter += n;
    fRayleigh2 += n * n;
  };
  void AddAbsorption(G4double n)
  {
    fAbsorptionCounter += n;
    fAbsorption2 += n * n;
  };
  void AddMie(G4double n)
  {
    fMieCounter += n;
    fMie2 += n * n;
  };
  void AddBoundary(G4double n)
  {
    fBoundaryCounter += n;
    fBoundary2 += n * n;
  };

  void Merge(const G4Run*) override;
  void EndOfRun();

 private:
  G4ParticleDefinition* fParticle = nullptr;

  G4double fCerenkovCounter = 0.;
  G4double fCerenkov2 = 0.;
  G4double fScintillationCounter = 0.;
  G4double fScintillation2 = 0.;
  G4double fRayleighCounter = 0.;
  G4double fRayleigh2 = 0.;
  G4double fAbsorptionCounter = 0.;
  G4double fAbsorption2 = 0.;
  G4double fMieCounter = 0.;
  G4double fMie2 = 0.;
  G4double fBoundaryCounter = 0.;
  G4double fBoundary2 = 0.;
  G4double fEnergy = -1.;
};

#endif