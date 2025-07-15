/// \file B1/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the B1::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace B1
{

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun = new G4ParticleGun(n_particle);

  // Default particle: neutron
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("neutron");

  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // Get the envelope volume
  G4double envSizeXY = 0;
  G4double envSizeZ = 0;

  if (!fEnvelopeBox) {
    G4LogicalVolume* envLV = G4LogicalVolumeStore::GetInstance()->GetVolume("Envelope");
    if (envLV) fEnvelopeBox = dynamic_cast<G4Box*>(envLV->GetSolid());
  }

  if (fEnvelopeBox) {
    envSizeXY = fEnvelopeBox->GetXHalfLength() * 2.;
    envSizeZ = fEnvelopeBox->GetZHalfLength() * 2.;
  } else {
    G4ExceptionDescription msg;
    msg << "Envelope volume of box shape not found.\n";
    msg << "Perhaps you have changed geometry.\n";
    msg << "The gun will be placed at the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()", "MyCode0002", JustWarning, msg);
  }

  // Generate a Gaussian-distributed energy (centered at 14 MeV, sigma = 0.5 MeV)
  G4double meanEnergy = 14.0 * MeV;
  G4double sigmaEnergy = 0.15 * MeV;
  G4double energy = G4RandGauss::shoot(meanEnergy, sigmaEnergy);

  if (energy < 0) energy = 0.1 * MeV;

  fParticleGun->SetParticleEnergy(energy);

  // Generate a random position 
  G4double size = 0.005;
  G4double x0 = size * envSizeXY * (G4UniformRand() - 0.5);
  G4double y0 = size * envSizeXY * (G4UniformRand() - 0.5);
  G4double z0 = -0.5 * envSizeZ + 2.0 * cm;  

  fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
  fParticleGun->GeneratePrimaryVertex(event);
}

}  // namespace B1
