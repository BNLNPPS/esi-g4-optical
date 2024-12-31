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
                                               fDetectorLogical(nullptr),
                                               fDetectorMaterial(nullptr),
                                               fDetectorOpSurface(nullptr),
                                               fmpt(nullptr),
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
  // their properties are defined:
  ConstructMaterials();

  G4Material *air = G4Material::GetMaterial("G4_AIR");
  G4bool checkOverlaps = true;

  // **********************************************************************
  //
  // We'll build
  //      - the world
  //      - the apparatus
  //      - the optical detector

  G4VSolid *worldSolid = new G4Box("worldBox", 10.*m, 10.*m, 10.*m);
  G4LogicalVolume *worldLogical = new G4LogicalVolume(worldSolid, air, "worldLogical");
  G4VPhysicalVolume *worldPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(), worldLogical,
                      "worldPhysical", nullptr, false, 0, checkOverlaps);

  // The apparatus
  G4VSolid *apparatusSolid = new G4Box("apparatusBox", 9.*m, 9.*m, 9.*m);
  G4LogicalVolume *apparatusLogical =
    new G4LogicalVolume(apparatusSolid, air, "apparatusLogical");

  G4VPhysicalVolume* apparatusPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.),
                      apparatusLogical, "apparatusPhysical",
                      worldLogical, false, 0, checkOverlaps);

  // Detector
  G4VSolid *detectorSolid = new G4Box("detectorBox", 10.*cm, 10.*cm, 10.*cm);
  fDetectorLogical =
    new G4LogicalVolume(detectorSolid, fDetectorMaterial, "detectorLogical");
  G4VPhysicalVolume* detectorPhysical =
    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.),
                      fDetectorLogical, "detectorPhysical",
                      apparatusLogical, false, 0, checkOverlaps);


  // **********************************************************************
  // OPTIONAL: Example of a CAD-based geometry
  /*
  std::string cadfile = "../cad/example.stl";
  CADMesh *mesh = new CADMesh((char*)cadfile.c_str(), mm, G4ThreeVector(), false);
  G4VSolid* cad_example_solid = mesh->TessellatedMesh();
  G4LogicalVolume* cad_example_logic =
    new G4LogicalVolume(cad_example_solid, fDetectorMaterial,
                        "cad_example_log", 0, 0, 0);
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.),
                    cad_example_logic, "cad_example_phys",
                    fDetectorLogical, false, 0, checkOverlaps);
  */

  // **********************************************************************
  // *** NEW SURFACE DEFINITION (Replacing the old one) *******************
  //
  // This corresponds to your GDML snippet:
  //   <opticalsurface finish="0" model="0" name="medium_container_bs0x560299397a10" type="0" value="1">
  //       <property name="EFFICIENCY"   ref="EFFICIENCY0x560299349111"/>
  //       <property name="REFLECTIVITY" ref="REFLECTIVITY0x5602993a4e30"/>
  //   </opticalsurface>

  // Create the OpticalSurface (type="0" -> dielectric_metal, etc.)
  fDetectorOpSurface = new G4OpticalSurface("medium_container_bs0x560299397a10");
  fDetectorOpSurface->SetType(dielectric_metal);  // "type=0"
  fDetectorOpSurface->SetModel(unified);          // "model=0"
  fDetectorOpSurface->SetFinish(polished);        // "finish=0"
  fDetectorOpSurface->SetSigmaAlpha(0.0);

  // We attach the EFFICIENCY and REFLECTIVITY to a new MPT
  G4MaterialPropertiesTable* surfMPT = new G4MaterialPropertiesTable();

  //
  // 1) EFFICIENCY array (2.034 eV -> 11.6 eV, all = 1.0)
  //
  // The big list from your <matrix name="EFFICIENCY0x560299349111" .../>
  // For clarity, these are just shown as code arrays below.  You must
  // ensure each energy/value is placed in order.
  //
  std::vector<G4double> EFFenergy = {
    2.034*eV, 2.068*eV, 2.103*eV, 2.139*eV, 2.177*eV, 2.216*eV, 2.256*eV,
    2.298*eV, 2.341*eV, 2.386*eV, 2.433*eV, 2.481*eV, 2.532*eV, 2.585*eV,
    2.64*eV,  2.697*eV, 2.757*eV, 2.82*eV,  2.885*eV, 2.954*eV, 3.026*eV,
    3.102*eV, 3.181*eV, 3.265*eV, 3.353*eV, 3.446*eV, 3.545*eV, 3.649*eV,
    3.76*eV,  3.877*eV, 4.002*eV, 4.136*eV, 5.0*eV,   6.0*eV,   7.0*eV,
    8.0*eV,   9.0*eV,   9.0*eV,   10.0*eV,  11.0*eV,  11.6*eV
  };
  std::vector<G4double> EFFvals(EFFenergy.size(), 0.5); 
  // all 1.0 as per your GDML snippet

  // Add the EFFICIENCY property
  surfMPT->AddProperty("EFFICIENCY",
                       EFFenergy.data(), EFFvals.data(),
                       G4int(EFFenergy.size()));

  //
  // 2) REFLECTIVITY array
  //
  // The GDML snippet shows many pairs like "1.512e-06 0 1.51385e-06 0 ..."
  // which are suspiciously small numbers if they represent energies in eV.
  // They may be wavelengths in meters or something else.  If they are truly
  // energies in eV, you can add them “as-is”, but it is unusual for optical
  // ranges.  Below, we add them literally as provided (assuming they are
  // eV). If needed, convert them to eV or to your desired units:
  //
  std::vector<G4double> RFLenergy = {
    1.512*eV, /* ... huge omitted block ... */ 
    10.0664*eV
    // NOTE: In reality, paste the entire big list of energies from your snippet
  };
  std::vector<G4double> RFLvals(RFLenergy.size(), 1.0); 
  // The snippet has all second-column = 0, so reflectivity = 0.0 at each point

  // Add the REFLECTIVITY property
  surfMPT->AddProperty("REFLECTIVITY",
                       RFLenergy.data(), RFLvals.data(),
                       G4int(RFLenergy.size()));

  // Finally attach the MPT to the optical surface
  fDetectorOpSurface->SetMaterialPropertiesTable(surfMPT);

  // Create border surfaces so that the new optical surface is “seen” at
  // the boundary between the detector and the apparatus:
  new G4LogicalBorderSurface("DetToApp",
                             detectorPhysical, apparatusPhysical,
                             fDetectorOpSurface);
  new G4LogicalBorderSurface("AppToDet",
                             apparatusPhysical, detectorPhysical,
                             fDetectorOpSurface);
  // **********************************************************************


  // Visualization attributes
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

  G4NistManager *nistManager = G4NistManager::Instance();
  // Build the default G4_AIR material from the NIST database
  G4Material* air = nistManager->FindOrBuildMaterial("G4_AIR");

  // --- Example: Add refractive index to G4_AIR ---
  {
    // Provide an energy range where you want a definite RINDEX
    // (2 eV to 8 eV is common for visible/UV photons).
    std::vector<G4double> airEnergy = {
      2.0*eV, 3.0*eV, 4.0*eV, 5.0*eV, 6.0*eV, 7.0*eV, 8.0*eV
    };
    // Typically, the refractive index of air is ~1.0 at optical energies.
    // You can choose 1.0003 for more realism, or just 1.0
    std::vector<G4double> airRIndex(airEnergy.size(), 1.0003);

    // Create a new MaterialPropertiesTable (MPT) for air
    G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();
    // Attach the RINDEX property
    airMPT->AddProperty("RINDEX",
                        airEnergy.data(), airRIndex.data(),
                        (G4int)airEnergy.size());

    // Assign this MPT to the “air” material
    air->SetMaterialPropertiesTable(airMPT);
  }

  // --- Example: Make a Water material with NIST elements ---
  auto H = new G4Element("Hydrogen","H", z=1, a=1.01*g/mole);
  auto O = new G4Element("Oxygen",  "O", z=8, a=16.00*g/mole);
  fDetectorMaterial = new G4Material("Water", density=1.0*g/cm3, nelements=2);
  fDetectorMaterial->AddElement(H,2);
  fDetectorMaterial->AddElement(O,1);

  // Optionally give Water its own MaterialPropertiesTable
  fmpt = new G4MaterialPropertiesTable();

  // For example, a generic RINDEX for water (1.33)
  {
    std::vector<G4double> waterEnergy = {
      2.0*eV, 3.0*eV, 4.0*eV, 5.0*eV, 6.0*eV, 7.0*eV, 8.0*eV
    };
    std::vector<G4double> waterRIndex(waterEnergy.size(), 1.33);

    fmpt->AddProperty("RINDEX",
                      waterEnergy.data(), waterRIndex.data(),
                      (G4int)waterEnergy.size());
  }

  fDetectorMaterial->SetMaterialPropertiesTable(fmpt);

  // If you want, set Birks Constant for Water scint:
  // fDetectorMaterial->GetIonisation()->SetBirksConstant(0.126*mm/MeV);
}