#ifndef RunAction_h
#define RunAction_h 1

#include "globals.hh"
#include "G4UserRunAction.hh"
#include <string>

class PrimaryGeneratorAction;
class Run;

// ---

class G4Run;

class RunAction : public G4UserRunAction
{
 public:
  RunAction(PrimaryGeneratorAction* = nullptr, std::string = "csv");
  ~RunAction() override;

  G4Run* GenerateRun() override;
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

 private:
  std::string _OutPutFormat;
  G4String fileName;
  Run* fRun = nullptr;
  PrimaryGeneratorAction* fPrimary = nullptr;
};

#endif