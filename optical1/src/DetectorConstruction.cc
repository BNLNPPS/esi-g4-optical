#include "DetectorConstruction.hh"
#include "OpticalMessenger.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

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

#include "mpt.hh"


// optional: #include "CADMesh.hh"

//---

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction(),
                                               fDetectorLogical(0),
                                               fDetectorMaterial(0),
                                               fDetectorOpSurface(0),
                                               fmpt(0),
                                               fVisAttributes()
{
  fMessenger = new OpticalMessenger(this);
}

//---

DetectorConstruction::~DetectorConstruction() {
  delete fMessenger;
  for (G4int i = 0; i < G4int(fVisAttributes.size()); ++i)
  {
    delete fVisAttributes[i];
  }
}

// ---

G4VPhysicalVolume *DetectorConstruction::Construct() {
  // NB. This is an important method as custom materials with
  // their propterties are defined:
  ConstructMaterials();

  G4Material *air = G4Material::GetMaterial("G4_AIR");

  G4bool checkOverlaps = true;


  // **********************************************************************
  //
  // We'll build
  //      - the world
  //      - the apparatus
  //      - the optical detector

  G4VSolid *worldSolid              = new G4Box("worldBox", 10.*m, 10.*m, 10.*m);
  G4LogicalVolume *worldLogical     = new G4LogicalVolume(worldSolid, air, "worldLogical");
  G4VPhysicalVolume *worldPhysical  = new G4PVPlacement(0, G4ThreeVector(), worldLogical, "worldPhysical", 0, false, 0, checkOverlaps);

  // The apparatus
  G4VSolid *apparatusSolid          = new G4Box("apparatusBox", 9.*m, 9.*m, 9.*m);
  G4LogicalVolume *apparatusLogical = new G4LogicalVolume(apparatusSolid, air, "apparatusLogical");

  new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), apparatusLogical, "apparatusPhysical", worldLogical, false, 0, checkOverlaps);

  // Detector
  
  G4VSolid *detectorSolid = new G4Box("detectorBox", 10. * cm, 10. * cm, 10. * cm);
  fDetectorLogical = new G4LogicalVolume(detectorSolid, fDetectorMaterial, "detectorLogical");
  new G4PVPlacement(0, G4ThreeVector(0., 0., 0. * m), fDetectorLogical, "detectorPhysical", apparatusLogical, false, 0, checkOverlaps);


  // CAD example

  /*
  std::string cadfile = "../cad/example.stl";
  CADMesh *mesh = new CADMesh((char*)cadfile.c_str(), mm, G4ThreeVector(), false);
  G4VSolid* cad_example_solid = mesh->TessellatedMesh();
  G4LogicalVolume* cad_example_logic = new G4LogicalVolume(cad_example_solid, fDetectorMaterial, "cad_example_log", 0, 0, 0);
  new G4PVPlacement(0, G4ThreeVector(0., 0., 0. * m), cad_example_logic, "cad_example_phys", fDetectorLogical, false, 0, checkOverlaps);
  */

  // FIXME -- surfaces added here, from OpNoviceDetectorCtor

  fDetectorOpSurface = new G4OpticalSurface("WaterSurface");
  fDetectorOpSurface->SetType(dielectric_LUTDAVIS);
  fDetectorOpSurface->SetFinish(Rough_LUT);
  fDetectorOpSurface->SetModel(DAVIS);


  // **********************************************************************
  // **********************************************************************
  // **********************************************************************
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
  fDetectorLogical->SetVisAttributes(visAttributes);
  fVisAttributes.push_back(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(0.8888, 0.8888, 0.0));
  visAttributes->SetVisibility(false);
  fVisAttributes.push_back(visAttributes);

  return worldPhysical; // return the world physical volume
}

// ---

void DetectorConstruction::ConstructSDandField() {}

// ---

void DetectorConstruction::ConstructMaterials() {
  G4double a, z, density;
  G4int nelements;
  
  G4double energyArray[]{ 2.034 * eV, 4.136 * eV };
  G4int lenArray = 2;

  // ---
  G4NistManager *nistManager = G4NistManager::Instance();

  nistManager->FindOrBuildMaterial("G4_AIR"); // Air

  G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;

  // -mxp- This is from OpNovice
  // Water
  auto H = new G4Element("Hydrogen",  "H", z = 1, a = 1.01 * g  / mole);
  auto O = new G4Element("Oxygen",    "O", z = 8, a = 16.00 * g / mole);
  fDetectorMaterial = new G4Material("Water", density = 1.0 * g / cm3, nelements = 2);
  fDetectorMaterial->AddElement(H, 2);
  fDetectorMaterial->AddElement(O, 1);

  fmpt = new G4MaterialPropertiesTable();

  // Values can be added to the material property table individually.
  // With this method, spline interpolation cannot be set. Arguments
  // createNewKey and spline both take their default values of false.
  // Need to specify the number of entries (1) in the arrays, as an argument
  // to AddProperty.
  

  // -mxp- this original code throws a warning exception, but apparently you need to "init" a property first.
  G4int numEntries = 1;
  fmpt->AddProperty("RINDEX", &photonEnergy[0], &refractiveIndex1[0], numEntries);


  for(size_t i = 1; i < photonEnergy.size(); ++i) {fmpt->AddEntry("RINDEX", photonEnergy[i], refractiveIndex1[i]);}


  // Check that group velocity is calculated from RINDEX
  if(fmpt->GetProperty("RINDEX")->GetVectorLength() != fmpt->GetProperty("GROUPVEL")->GetVectorLength()) {
    G4ExceptionDescription ed;
    ed << "Error calculating group velocities. Incorrect number of entries in group velocity material property vector.";
    G4Exception("DetectorConstruction", "optical1", FatalException, ed);
  }

  // Adding a property from two std::vectors. Argument createNewKey is false and spline is true.
  fmpt->AddProperty("ABSLENGTH", photonEnergy, absorption, false, true);

  fmpt->AddProperty("MIEHG", energy_water, mie_water, false, true);
  fmpt->AddConstProperty("MIEHG_FORWARD", mie_water_const[0]);
  fmpt->AddConstProperty("MIEHG_BACKWARD", mie_water_const[1]);
  fmpt->AddConstProperty("MIEHG_FORWARD_RATIO", mie_water_const[2]);


  // FIXME Better make a flag to print the necessary info instead of comment/uncomment
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  // G4cout << "Water G4MaterialPropertiesTable:" << G4endl;
  // fmpt->DumpTable();

  fDetectorMaterial->SetMaterialPropertiesTable(fmpt);

  // Set the Birks Constant for the Water scintillator
  fDetectorMaterial->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

}

  // ATTIC
  // See nistManager in the code, this part is retired for now
  // nistManager->FindOrBuildMaterial("G4_Ar");                      // Argon gas
  // nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"); // C_9H_10

  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  // int L = photonEnergy.size();
  // std::cout << L << std::endl;
  // for (G4int i = 0; i < G4int(photonEnergy.size()); ++i) {
  //   std::cout << photonEnergy[i] << std::endl;
  // }