#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

// ---

PrimaryGeneratorAction::PrimaryGeneratorAction(): G4VUserPrimaryGeneratorAction(),     
  fParticleGun(0), fMessenger(0), 
  fPositron(0), fElectron(0), fMuon(0), fPion(0), fKaon(0), fProton(0),
  fMomentum(1000.*MeV),
  fSigmaMomentum(50.*MeV),
  fSigmaAngle(2.*deg),
  fRandomizePrimary(false)
{
    G4int n_particle = 1;
    fParticleGun  = new G4ParticleGun(n_particle);
    
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName;
    fElectron   = particleTable->FindParticle(particleName="e-");
    fPositron   = particleTable->FindParticle(particleName="e+");
    fMuon       = particleTable->FindParticle(particleName="mu+");
    fPion       = particleTable->FindParticle(particleName="pi+");
    fKaon       = particleTable->FindParticle(particleName="kaon+");
    fProton     = particleTable->FindParticle(particleName="proton");
    
    // default particle kinematics
    fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,-10.*cm));
    fParticleGun->SetParticleDefinition(fProton);
    
    // define commands for this class
    DefineCommands();
}

// ---

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
    delete fMessenger;
}

// ---

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    G4ParticleDefinition* particle;
    
    if (fRandomizePrimary)
    {
        G4int i = (int)(5.*G4UniformRand());
        switch(i)
        {
            case 0:
                particle = fPositron;
                break;
            case 1:
                particle = fMuon;
                break;
            case 2:
                particle = fPion;
                break;
            case 3:
                particle = fKaon;
                break;
            default:
                particle = fProton;
                break;
        }
        fParticleGun->SetParticleDefinition(particle);
    }
    else
    {
        particle = fParticleGun->GetParticleDefinition();
    }
    
    G4double pp = fMomentum + (G4UniformRand()-0.5)*fSigmaMomentum;
    G4double mass = particle->GetPDGMass();
    G4double Ekin = std::sqrt(pp*pp+mass*mass)-mass;
    fParticleGun->SetParticleEnergy(Ekin);
    
    G4double angle = (G4UniformRand()-0.5)*fSigmaAngle;
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(std::sin(angle),0.,
                                                             std::cos(angle)));
    
    fParticleGun->GeneratePrimaryVertex(event);
}

// ---

void PrimaryGeneratorAction::DefineCommands()
{
    // Define //generator command directory using generic messenger class
    fMessenger 
      = new G4GenericMessenger(this, 
                               "/tutorial/generator/", 
                               "Primary generator control");
              
    // momentum command
    G4GenericMessenger::Command& momentumCmd
      = fMessenger->DeclarePropertyWithUnit("momentum", "GeV", fMomentum, 
                                    "Mean momentum of primaries.");
    momentumCmd.SetParameterName("p", true);
    momentumCmd.SetRange("p>=0.");                                
    momentumCmd.SetDefaultValue("1.");
    // ok
    //momentumCmd.SetParameterName("p", true);
    //momentumCmd.SetRange("p>=0.");                                
    
    // sigmaMomentum command
    G4GenericMessenger::Command& sigmaMomentumCmd
      = fMessenger->DeclarePropertyWithUnit("sigmaMomentum",
          "MeV", fSigmaMomentum, "Sigma momentum of primaries.");
    sigmaMomentumCmd.SetParameterName("sp", true);
    sigmaMomentumCmd.SetRange("sp>=0.");                                
    sigmaMomentumCmd.SetDefaultValue("50.");

    // sigmaAngle command
    G4GenericMessenger::Command& sigmaAngleCmd
      = fMessenger->DeclarePropertyWithUnit("sigmaAngle", "rad", fSigmaAngle, 
                                    "Sigma angle divergence of primaries.");
    sigmaAngleCmd.SetParameterName("t", true);
    sigmaAngleCmd.SetRange("t>=0.");                                
    sigmaAngleCmd.SetDefaultValue("2.");

    // randomizePrimary command
    G4GenericMessenger::Command& randomCmd
      = fMessenger->DeclareProperty("randomizePrimary", fRandomizePrimary);
    G4String guidance
       = "Boolean flag for randomizing primary particle types.\n";   
    guidance
       += "In case this flag is false, you can select the primary particle\n";
    guidance += "  with /gun/particle command.";                               
    randomCmd.SetGuidance(guidance);
    randomCmd.SetParameterName("flg", true);
    randomCmd.SetDefaultValue("true");
}
