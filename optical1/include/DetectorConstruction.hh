#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include <CLHEP/Units/SystemOfUnits.h>

#include "G4LogicalVolume.hh"
#include "G4Material.hh"

#include "G4OpticalSurface.hh"
#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"

#include <vector>

// -mxp-
class OpticalMessenger;

class G4VPhysicalVolume;
class G4Material;
class G4VisAttributes;
class G4GenericMessenger;

// ---
class DetectorConstruction: public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();
    
    //Here the geomtry is constructed. This method is called only by
    //master thread and all geometry built here is shared among threads
    virtual G4VPhysicalVolume* Construct();
    
    //Some components of geometry are thread pricate: SensitiveDetectors and megnetic field.
    //Build them here because this method is called by each thread
    virtual void ConstructSDandField();

    //This is just a convinience: a method where all materials needed are created
    void ConstructMaterials();
    
private:

    // -mxp-
    // G4GenericMessenger* fMessenger;

    OpticalMessenger*   fMessenger;
    G4LogicalVolume*    fDetectorLogical;
    G4Material*         fDetectorMaterial;
    G4OpticalSurface*   fDetectorSurface;


    G4MaterialPropertiesTable* fmpt;
    
    std::vector<G4VisAttributes*> fVisAttributes;

};

#endif
