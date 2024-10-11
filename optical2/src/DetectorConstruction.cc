#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


#include "mpt.hh" //local material properties table

// ---
G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;


G4VPhysicalVolume* DetectorConstruction::Construct() {
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

// --
void DetectorConstruction::DefineMaterials() {
  // Preliminary setup
  G4double a, z, density;
  G4int nelements;
  
  G4double energyArray[]{ 2.034 * eV, 4.136 * eV };
  G4int lenArray = 2;

  // --
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Pb");
  nistManager->FindOrBuildMaterial("G4_AIR"); // Air

  // Water
  auto H = new G4Element("Hydrogen",  "H", z = 1, a = 1.01 * g  / mole);
  auto O = new G4Element("Oxygen",    "O", z = 8, a = 16.00 * g / mole);
  _DetectorMaterial = new G4Material("Water", density = 1.0 * g / cm3, nelements = 2);
  _DetectorMaterial->AddElement(H, 2);
  _DetectorMaterial->AddElement(O, 1);

  _mpt = new G4MaterialPropertiesTable();  

  // -mxp- this original code throws a warning exception, but apparently you need to "init" a property first.
  G4int numEntries = 1;
  _mpt->AddProperty("RINDEX", &photonEnergy[0], &refractiveIndex1[0], numEntries);  

  for(size_t i = 1; i < photonEnergy.size(); ++i) {_mpt->AddEntry("RINDEX", photonEnergy[i], refractiveIndex1[i]);}

  // Check that group velocity is calculated from RINDEX
  if(_mpt->GetProperty("RINDEX")->GetVectorLength() != _mpt->GetProperty("GROUPVEL")->GetVectorLength()) {
    G4ExceptionDescription ed;
    ed << "Error calculating group velocities. Incorrect number of entries in group velocity material property vector.";
    G4Exception("DetectorConstruction", "optical2", FatalException, ed);
  }

  // Adding a property from two std::vectors. Argument createNewKey is false and spline is true.
  _mpt->AddProperty("ABSLENGTH", photonEnergy, absorption, false, true);

  _mpt->AddProperty("MIEHG", energy_water, mie_water, false, true);
  _mpt->AddConstProperty("MIEHG_FORWARD", mie_water_const[0]);
  _mpt->AddConstProperty("MIEHG_BACKWARD", mie_water_const[1]);
  _mpt->AddConstProperty("MIEHG_FORWARD_RATIO", mie_water_const[2]);


  // FIXME Better make a flag to print the necessary info instead of comment/uncomment
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  G4cout << "Water G4MaterialPropertiesTable:" << G4endl;
  _mpt->DumpTable();

  _DetectorMaterial->SetMaterialPropertiesTable(_mpt);

  // Set the Birks Constant for the Water scintillator
  _DetectorMaterial->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);


  // #######################################################################
  // #######################################################################
  // #######################################################################
  // Liquid argon material

  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}
// ---

G4VPhysicalVolume* DetectorConstruction::DefineVolumes() {

  G4Material *air = G4Material::GetMaterial("G4_AIR");
  G4bool checkOverlaps = true;


  // **********************************************************************
  //
  // We'll build: - the world, - the apparatus, - the optical detector

  G4VSolid *worldSolid              = new G4Box("worldBox", 10.*m, 10.*m, 10.*m);
  G4LogicalVolume *worldLogical     = new G4LogicalVolume(worldSolid, air, "worldLogical");
  G4VPhysicalVolume *worldPhysical  = new G4PVPlacement(0, G4ThreeVector(), worldLogical, "worldPhysical", 0, false, 0, checkOverlaps);

  // The apparatus
  G4VSolid *apparatusSolid          = new G4Box("apparatusBox", 9.*m, 9.*m, 9.*m);
  G4LogicalVolume *apparatusLogical = new G4LogicalVolume(apparatusSolid, air, "apparatusLogical");

  new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), apparatusLogical, "apparatusPhysical", worldLogical, false, 0, checkOverlaps);

  // Detector
  G4VSolid *detectorSolid = new G4Box("detectorBox", 10. * cm, 10. * cm, 10. * cm);
  _DetectorLogical = new G4LogicalVolume(detectorSolid, _DetectorMaterial, "detectorLogical");
  new G4PVPlacement(0, G4ThreeVector(0., 0., 0. * m), _DetectorLogical, "detectorPhysical", apparatusLogical, false, 0, checkOverlaps);


  // According to the "OpNoviceDetectorCtor"
  _DetectorOpSurface = new G4OpticalSurface("WaterSurface");
  _DetectorOpSurface->SetType(dielectric_LUTDAVIS);
  _DetectorOpSurface->SetFinish(Rough_LUT);
  _DetectorOpSurface->SetModel(DAVIS);

  // visualization attributes

  G4VisAttributes *visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  visAttributes->SetVisibility(false);
  worldLogical->SetVisAttributes(visAttributes);
  _VisAttributes.push_back(visAttributes);
  delete(visAttributes);

  visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  visAttributes->SetVisibility(false);
  apparatusLogical->SetVisAttributes(visAttributes);
  _VisAttributes.push_back(visAttributes);

  // visAttributes = new G4VisAttributes(G4Colour(0.8888, 0.0, 0.0));
  // _DetectorLogical->SetVisAttributes(visAttributes);
  // _VisAttributes.push_back(visAttributes);

  // visAttributes = new G4VisAttributes(G4Colour(0.8888, 0.8888, 0.0));
  // visAttributes->SetVisibility(false);
  // _VisAttributes.push_back(visAttributes);


  return worldPhysical; // return the world physical volume

  // Work in progress, note that the code below is not supposed to run for now

  
  // Geometry parameters
  G4int    nofLayers      = 10;
  G4double absoThickness  = 10.*mm;
  G4double gapThickness   =  5.*mm;
  G4double calorSizeXY    = 10.*cm;

  auto layerThickness = absoThickness + gapThickness;
  auto calorThickness = nofLayers * layerThickness;
  auto worldSizeXY = 1.2 * calorSizeXY;
  auto worldSizeZ  = 1.2 * calorThickness;

  // Get materials
  auto defaultMaterial  = G4Material::GetMaterial("Galactic");
  auto absorberMaterial = G4Material::GetMaterial("G4_Pb");
  auto gapMaterial      = G4Material::GetMaterial("liquidArgon");

  if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()", "MyCode0001", FatalException, msg);
  }

  // ---
  // World
  auto worldS
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size

  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name

  auto worldPV = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                         // at (0,0,0)
    worldLV,                                 // its logical volume
    "World",                                 // its name
    nullptr,                                 // its mother  volume
    false,                                   // no boolean operation
    0,                                       // copy number
    fCheckOverlaps);                         // checking overlaps

  // ---
  // Calorimeter
  auto calorimeterS
    = new G4Box("Calorimeter",     // its name
                 calorSizeXY/2, calorSizeXY/2, calorThickness/2); // its size

  auto calorLV
    = new G4LogicalVolume(
                 calorimeterS,     // its solid
                 defaultMaterial,  // its material
                 "Calorimeter");   // its name

  new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),          // at (0,0,0)
    calorLV,                  // its logical volume
    "Calorimeter",            // its name
    worldLV,                  // its mother  volume
    false,                    // no boolean operation
    0,                        // copy number
    fCheckOverlaps);          // checking overlaps

  // ---
  // Layer
  auto layerS
    = new G4Box("Layer",           // its name
                 calorSizeXY/2, calorSizeXY/2, layerThickness/2); // its size

  auto layerLV
    = new G4LogicalVolume(
                 layerS,           // its solid
                 defaultMaterial,  // its material
                 "Layer");         // its name

  new G4PVReplica(
                 "Layer",          // its name
                 layerLV,          // its logical volume
                 calorLV,          // its mother
                 kZAxis,           // axis of replication
                 nofLayers,        // number of replica
                 layerThickness);  // witdth of replica

  // ---
  // Absorber
  auto absorberS
    = new G4Box("Abso",            // its name
                 calorSizeXY/2, calorSizeXY/2, absoThickness/2); // its size

  auto absorberLV
    = new G4LogicalVolume(
                 absorberS,        // its solid
                 absorberMaterial, // its material
                 "Abso");          // its name

  fAbsorberPV = new G4PVPlacement(nullptr,     // no rotation
    G4ThreeVector(0., 0., -gapThickness / 2),  // its position
    absorberLV,                                // its logical volume
    "Abso",                                    // its name
    layerLV,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);                           // checking overlaps

  //
  // Gap
  //
  auto gapS
    = new G4Box("Gap",             // its name
                 calorSizeXY/2, calorSizeXY/2, gapThickness/2); // its size

  auto gapLV
    = new G4LogicalVolume(
                 gapS,             // its solid
                 gapMaterial,      // its material
                 "Gap");           // its name

  fGapPV = new G4PVPlacement(nullptr,          // no rotation
    G4ThreeVector(0., 0., absoThickness / 2),  // its position
    gapLV,                                     // its logical volume
    "Gap",                                     // its name
    layerLV,                                   // its mother  volume
    false,                                     // no boolean operation
    0,                                         // copy number
    fCheckOverlaps);                           // checking overlaps


  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "---> The calorimeter is " << nofLayers << " layers of: [ "
    << absoThickness/mm << "mm of " << absorberMaterial->GetName()
    << " + "
    << gapThickness/mm << "mm of " << gapMaterial->GetName() << " ] " << G4endl
    << "------------------------------------------------------------" << G4endl;

  // --
  // Visualization attributes
  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  calorLV->SetVisAttributes(G4VisAttributes(G4Colour::White()));

  // Always return the physical World !!!!!!!!!!!!!!!!!!!!!!!!!!
  //
  return worldPV;
}

// ---

void DetectorConstruction::ConstructSDandField()
{
  // Uniform magnetic field is then created automatically if  the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}


