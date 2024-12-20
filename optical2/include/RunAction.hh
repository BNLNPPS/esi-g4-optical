#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/// Run action class
///
/// It accumulates statistic and computes dispersion of the energy deposit
/// and track lengths of charged particles with use of analysis tools:
/// H1D histograms are created in BeginOfRunAction() for the following
/// physics quantities:
/// - Edep in absorber
/// - Edep in gap
/// - Track length in absorber
/// - Track length in gap
/// The same values are also saved in the ntuple.
/// The histograms and ntuple are saved in the output file in a format
/// according to a specified file extension.
///
/// In EndOfRunAction(), the accumulated statistic and computed
/// dispersion is printed.
///

class RunAction : public G4UserRunAction
{
  public:
    RunAction(const G4String fn);
    ~RunAction() override = default;

    G4Run* GenerateRun() override;

    void    BeginOfRunAction(const G4Run*) override;
    void    EndOfRunAction(const G4Run*) override;

    // void    SetFilename(const G4String fn) {_filename=fn;}

  private:
      G4String  _filename;
};

#endif

