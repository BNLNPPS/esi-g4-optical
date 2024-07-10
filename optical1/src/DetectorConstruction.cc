#include "DetectorConstruction.hh"
#include "OpticalMessenger.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4UserLimits.hh"

#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

//---

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction(), fdetectorLogical(0), fVisAttributes() {fMessenger = new OpticalMessenger(this);}

//---

DetectorConstruction::~DetectorConstruction()
{
  delete fMessenger;
  for (G4int i = 0; i < G4int(fVisAttributes.size()); ++i)
  {
    delete fVisAttributes[i];
  }
}

// ---

G4VPhysicalVolume *DetectorConstruction::Construct()
{
  // Construct materials
  ConstructMaterials();
  G4Material *air = G4Material::GetMaterial("G4_AIR");
  G4Material *argonGas = G4Material::GetMaterial("G4_Ar");
  G4Material *scintillator = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  G4bool checkOverlaps = true;

  G4VSolid *worldSolid = new G4Box("worldBox", 5. * m, 5. * m, 5. * m);

  G4LogicalVolume *worldLogical = new G4LogicalVolume(worldSolid, air, "worldLogical");
  G4VPhysicalVolume *worldPhysical = new G4PVPlacement(0, G4ThreeVector(), worldLogical, "worldPhysical", 0, false, 0, checkOverlaps);

  // The apparatus
  G4VSolid *apparatusSolid = new G4Box("apparatusBox", .5 * m, .5 * m, .5 * m);
  G4LogicalVolume *apparatusLogical = new G4LogicalVolume(apparatusSolid, air, "apparatusLogical");

  new G4PVPlacement(0, G4ThreeVector(0., 0., -5. * m), apparatusLogical, "apparatusPhysical", worldLogical, false, 0, checkOverlaps);

  // G4double x = 0. //-5.*m * std::sin(fArmAngle);
  // G4double z = 0. // 5.*m * std::cos(fArmAngle);

  // Detector
  G4VSolid *detectorSolid = new G4Box("detectorBox", 10. * cm, 10. * cm, 10. * cm);
  fdetectorLogical = new G4LogicalVolume(detectorSolid, scintillator, "detectorLogical");

  new G4PVPlacement(0, G4ThreeVector(0., 0., -1.5 * m), fdetectorLogical, "detectorPhysical", apparatusLogical, false, 0, checkOverlaps);

  // visualization attributes

  G4VisAttributes *visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  visAttributes->SetVisibility(false);
  worldLogical->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  visAttributes->SetVisibility(false);
  apparatusLogical->SetVisAttributes(visAttributes);

  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(0.8888, 0.0, 0.0));
  fdetectorLogical->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(0.8888, 0.8888, 0.0));
  visAttributes->SetVisibility(false);
  fVisAttributes.push_back(visAttributes);

  return worldPhysical; // return the world physical volume 
}

// ---

void DetectorConstruction::ConstructSDandField() {}

// ---

void DetectorConstruction::ConstructMaterials()
{
  G4NistManager *nistManager = G4NistManager::Instance();

  nistManager->FindOrBuildMaterial("G4_AIR");                     // Air
  nistManager->FindOrBuildMaterial("G4_Ar");                      // Argon gas
  nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); // C_9H_10

  G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}
