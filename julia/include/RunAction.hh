#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <julia.h>

class G4Run;

///

class RunAction : public G4UserRunAction
{
  public:
    RunAction(const G4String fn, bool master=false);
    ~RunAction() override = default;

    G4Run* GenerateRun() override;

    void    BeginOfRunAction(const G4Run*) override;
    void    EndOfRunAction(const G4Run*) override;

  private:
      G4String  _filename;
      bool      _master=false;
      jl_function_t * begin_run_jl  = NULL;
      jl_function_t * end_run_jl    = NULL;
};

#endif

