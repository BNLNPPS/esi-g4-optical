#ifndef RunAction_h
#define RunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include <vector>

class PrimaryGeneratorAction;
class Run;

// ---

class G4Run;

class RunAction : public G4UserRunAction
{
 public:
  RunAction(PrimaryGeneratorAction* = nullptr);
  ~RunAction() override;

  G4Run* GenerateRun() override;
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;
  void AddPhoton(std::vector<double>);

  unsigned int printOutBufferSize;
  std::vector<std::vector<double>> PhotonData;

 private:
  Run* fRun = nullptr;
  PrimaryGeneratorAction* fPrimary = nullptr;
};

#endif