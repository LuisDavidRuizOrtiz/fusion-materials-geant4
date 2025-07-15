/// \file B1/include/PrimaryGeneratorAction.hh
/// \brief Definition of the B1::PrimaryGeneratorAction class

#ifndef B1PrimaryGeneratorAction_h
#define B1PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;
class G4Box;

namespace B1
{

/// The primary generator action class with particle gun.
///
/// Default: 14 MeV neutrons, Gaussian-distributed energy,
/// launched randomly over 80% of the envelope front face.

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event*) override;

    const G4ParticleGun* GetParticleGun() const { return fParticleGun; }

  private:
    G4ParticleGun* fParticleGun = nullptr;
    G4Box* fEnvelopeBox = nullptr;
};

}  // namespace B1

#endif
