#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"


#include <CLHEP/Units/SystemOfUnits.h>

#include "G4LogicalVolume.hh"
#include "G4Material.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh" // #include "G4LogicalSkinSurface.hh"

#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"

// ----------------------------------------------------------------------------------------
class OpticalMessenger;

class G4VPhysicalVolume;
class G4Material;
class G4VisAttributes;
class G4GenericMessenger;

class G4GlobalMagFieldMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction {
  public:
    DetectorConstruction()            = default;
    ~DetectorConstruction() override  = default;

  public:
    G4VPhysicalVolume* Construct()  override;
    void ConstructSDandField()      override;

    // get methods
    const G4VPhysicalVolume* GetAbsorberPV() const;
    const G4VPhysicalVolume* GetGapPV() const;

  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    // ----------------------------------------------------------------------------------------
    // data members
    // -mxp-
    G4MaterialPropertiesTable* _mpt;
    OpticalMessenger*   _OpMessenger;
    G4LogicalVolume*    _DetectorLogical;
    G4Material*         _DetectorMaterial;
    G4OpticalSurface*   _DetectorOpSurface;    

    std::vector<G4VisAttributes*> _VisAttributes;
    // end -mxp-


    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;  // magnetic field messenger

    G4VPhysicalVolume* fAbsorberPV = nullptr; // the absorber physical volume
    G4VPhysicalVolume* fGapPV = nullptr;      // the gap physical volume

    G4bool fCheckOverlaps = true; // option to activate checking of volumes overlaps

};



// ------------------------------------------------------------------------------------
// inline functions

inline const G4VPhysicalVolume* DetectorConstruction::GetAbsorberPV() const {return fAbsorberPV;}
inline const G4VPhysicalVolume* DetectorConstruction::GetGapPV()      const {return fGapPV;}


#endif

