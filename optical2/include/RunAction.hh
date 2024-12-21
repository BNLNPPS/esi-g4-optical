#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

///

class RunAction : public G4UserRunAction
{
  public:
    RunAction(const G4String fn);
    ~RunAction() override = default;

    G4Run* GenerateRun() override;

    void    BeginOfRunAction(const G4Run*) override;
    void    EndOfRunAction(const G4Run*) override;

  private:
      G4String  _filename;
};

#endif

