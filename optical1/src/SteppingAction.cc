#include "SteppingAction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4MTRunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "RunAction.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

SteppingAction::SteppingAction(RunAction* runAction)
:runAction(runAction){}

SteppingAction::~SteppingAction(){}

void SteppingAction::UserSteppingAction(const G4Step* step)
{

G4Track* theTrack = step->GetTrack();
if(theTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition())
    {
    if(theTrack->GetCreatorProcess()->GetProcessName()=="Cerenkov") {
        
      //  auto runAction = static_cast<RunAction*>(G4MTRunManager::GetMasterRunManager()->GetUserRunAction());

        /*
        if (runAction) {
            G4cout << "RunAction pointer OK" << G4endl;
            }           else      {
            G4cerr << "RunAction is not available or not of type RunAction." << G4endl;
            }
        */

        double x = step->GetPostStepPoint()->GetPosition().x();
        double y = step->GetPostStepPoint()->GetPosition().y();
        double z = step->GetPostStepPoint()->GetPosition().z();
        double x_dir = step->GetTrack()->GetMomentumDirection()[0];
        double y_dir = step->GetTrack()->GetMomentumDirection()[1];
        double z_dir = step->GetTrack()->GetMomentumDirection()[2];
        double energy = step->GetPostStepPoint()->GetKineticEnergy();
        std::vector<double> Photon = {x, y, z, x_dir, y_dir, z_dir, energy};
        runAction->AddPhoton(Photon);
        theTrack->SetTrackStatus(fStopAndKill);
    }}


}
