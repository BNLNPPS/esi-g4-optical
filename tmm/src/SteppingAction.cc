#include "SteppingAction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4MTRunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "RunAction.hh"
#include "tmm.h"
#include "G4AnalysisManager.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"

SteppingAction::SteppingAction(RunAction* runAction)
:runAction(runAction){}

SteppingAction::~SteppingAction(){}

void SteppingAction::UserSteppingAction(const G4Step* step) {

    G4Track* theTrack = step->GetTrack();
   

    StackSpec<double, 2> ss = StackSpec<double,2>::Create2(1.0f, 1.5f);
    double wavelength_nm = 500.0;
    double minus_cos_theta = -1.0;
    Stack<double, 2> stack(wavelength_nm, minus_cos_theta, ss);
    std::cout << stack << std::endl;
    
    std::cout << "Final Reflectance (S): " << stack.art.R_s << std::endl;
    std::cout << "Final Reflectance (P): " << stack.art.R_p << std::endl;
    std::cout << "Final Transmittance (S): " << stack.art.T_s << std::endl;
    std::cout << "Final Transmittance (P): " << stack.art.T_p << std::endl;
    std::cout << "Final Absorption (S): " << stack.art.A_s << std::endl;
    std::cout << "Final Absorption (P): " << stack.art.A_p << std::endl;


   /*
    if(theTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()) {
        if(theTrack->GetCreatorProcess()->GetProcessName()=="Cerenkov") {
            //  auto runAction = static_cast<RunAction*>(G4MTRunManager::GetMasterRunManager()->GetUserRunAction());

            
            // if (runAction) {G4cout << "RunAction pointer OK" << G4endl;}
            // else      {     G4cerr << "RunAction is not available or not of type RunAction." << G4endl; }
            

            double x = step->GetPostStepPoint()->GetPosition().x();
            double y = step->GetPostStepPoint()->GetPosition().y();
            double z = step->GetPostStepPoint()->GetPosition().z();
            double x_dir = step->GetTrack()->GetMomentumDirection()[0];
            double y_dir = step->GetTrack()->GetMomentumDirection()[1];
            double z_dir = step->GetTrack()->GetMomentumDirection()[2];
            double energy = step->GetPostStepPoint()->GetKineticEnergy();

            auto analysisManager = G4AnalysisManager::Instance();
            if(analysisManager->IsOpenFile()) {
                //Fill the NTuple
                analysisManager->FillNtupleDColumn(0, energy);
                analysisManager->FillNtupleDColumn(1, x);
                analysisManager->FillNtupleDColumn(2, y);
                analysisManager->FillNtupleDColumn(3, z);
                analysisManager->FillNtupleDColumn(4, x_dir);
                analysisManager->FillNtupleDColumn(5, y_dir);
                analysisManager->FillNtupleDColumn(6, z_dir);
                analysisManager->AddNtupleRow();
            }

          
           // std::vector<double> Photon = {x, y, z, x_dir, y_dir, z_dir, energy};
           // runAction->AddPhoton(Photon);
           // theTrack->SetTrackStatus(fStopAndKill);
            

        }
    }

   */
}
