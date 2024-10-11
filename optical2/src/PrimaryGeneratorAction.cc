#include "PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

// ---
PrimaryGeneratorAction::PrimaryGeneratorAction() {
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic
  //
  auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(300.*MeV);
}

// ---

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  delete fParticleGun;
}

// ---

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume
  // from G4LogicalVolumeStore
  //

  G4double worldZHalfLength = 0.;
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("worldLogical");
  auto detecLV = G4LogicalVolumeStore::GetInstance()->GetVolume("detectorLogical");
  // Check that the world volume has box shape
  G4Box* worldBox = nullptr;
  if (  worldLV ) {
    worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
  }

  if ( worldBox ) {
    worldZHalfLength = worldBox->GetZHalfLength();
  }
  else  {
    G4ExceptionDescription msg;
    msg << "World volume of box shape not found." << G4endl;
    msg << "Perhaps you have changed geometry." << G4endl;
    msg << "The gun will be place in the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
      "MyCode0002", JustWarning, msg);
  }

  G4Box* detecBox = nullptr;
  detecBox = dynamic_cast<G4Box*>(detecLV->GetSolid());

  G4double detecZHalfLength = detecBox->GetZHalfLength();
  // Set gun position
  fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -detecZHalfLength));

  fParticleGun->GeneratePrimaryVertex(anEvent);
}

