#ifndef RunData_h
#define RunData_h 1

#include "G4Run.hh"
#include "globals.hh"

#include <array>

// ---

///  Run data class
///
/// The data are collected step by step in SteppingAction, and
/// the accumulated values are filled in histograms and a Ntuple
/// event by event in EventAction.

class RunData: public G4Run {

public:
  RunData() = default;
  ~RunData() override = default;

  void Add(G4int id, G4double de, G4double dl);
  void FillPerEvent();

  void Reset();

  // Get methods
  G4String  GetVolumeName(G4int id) const;
  G4double  GetEdep(G4int id) const;
  G4double  GetTrackLength(G4int id) const;

  void AddPhoton(); // {nPhotons+=1;}
  G4int GetNphotons() {return nPhotons;};

private:

  G4int nPhotons=0;

  // std::array<G4String, kDim>  fVolumeNames = { "Absorber", "Gap" };
  // std::array<G4double, kDim>  fEdep = { 0., 0. };
  // std::array<G4double, kDim>  fTrackLength = { 0., 0. };
};

// inline functions

// inline void RunData::Add(G4int id, G4double de, G4double dl) {
//   fEdep[id] += de;
//   fTrackLength[id] += dl;
// }

// inline G4String  RunData::GetVolumeName(G4int id) const {
//   return fVolumeNames[id];
// }

// inline G4double  RunData::GetEdep(G4int id) const {
//   return fEdep[id];
// }

// inline G4double  RunData::GetTrackLength(G4int id) const {
//   return fTrackLength[id];
// }

#endif

