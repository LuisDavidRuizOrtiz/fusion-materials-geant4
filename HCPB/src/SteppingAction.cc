#include "SteppingAction.hh"
#include "EventAction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4TouchableHandle.hh"
#include "G4ios.hh"

#include <fstream>

namespace B1
{

SteppingAction::SteppingAction(EventAction* eventAction)
  : G4UserSteppingAction(),
    fEventAction(eventAction)
{}

SteppingAction::~SteppingAction() = default;

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();

  auto preVolume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  auto postVolume = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();

  if (!preVolume || !postVolume) return;

  G4String preName  = preVolume->GetName();
  G4String postName = postVolume->GetName();

  G4double energy = track->GetKineticEnergy();
  constexpr G4double interfaceZ = -22.5 * cm;  // W front face

  // --- Record energy deposition
  G4double edep = step->GetTotalEnergyDeposit();
  if (edep > 0.) {
    auto edepVol = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume();
    if (edepVol) {
      G4String volName = edepVol->GetName();
      fEventAction->AddEdep(edep);
      fEventAction->AddEdepByVolume(volName, edep);
    }
  }

  // --- Neutron transitions across key interfaces
  if (particle->GetParticleName() == "neutron") {
    if (preName.find("Envelope") != std::string::npos &&
        postName.find("Plate1") != std::string::npos) {
      fEventAction->AddEnergyBeforeW(energy);
      fEventAction->IncrementNeutronInCount();
    }

    if (preName.find("Plate1") != std::string::npos &&
        postName.find("Envelope") != std::string::npos) {
      fEventAction->MarkBackscattered();
    }

    // After W: Plate1 → Be
    if (preName.find("Plate1") != std::string::npos &&
        postName.find("Be") != std::string::npos) {
      fEventAction->AddEnergyAfterW(energy);

      //  Only count neutron as effective if it enters the first Be layer
      if (postName == "Be1") {
        fEventAction->MarkEffectiveNeutron();
      }
    }

    // Before EUROFER: Li2TiO3 → Plate3
    if (preName.find("Li2TiO3") != std::string::npos &&
        postName.find("Plate3") != std::string::npos) {
      fEventAction->AddEnergyBeforeEUROFER(energy);
    }

    if (preName.find("Plate3") != std::string::npos &&
        postName.find("Envelope") != std::string::npos) {
      fEventAction->AddEnergyAfterEUROFER(energy);
    }

    // --- Neutron multiplication (n, kn)
    const auto* secondaries = step->GetSecondaryInCurrentStep();
    int neutronCount = 0;
    for (const auto* sec : *secondaries) {
      if (sec->GetDefinition()->GetParticleName() == "neutron") {
        ++neutronCount;
      }
    }

    if (neutronCount > 1) {
      for (const auto* sec : *secondaries) {
        if (sec->GetDefinition()->GetParticleName() == "neutron") {
          G4ThreeVector pos = sec->GetPosition();
          G4String volName = preName;
          G4double z_relative = pos.z() - interfaceZ;

          G4cout << "[MULT] Neutron multiplication (" << neutronCount
                 << " neutrons) in " << volName << " at "
                 << pos / cm << " cm" << G4endl;

          std::ofstream outMult("neutron_multiplication_depth.txt", std::ios::app);
          outMult << volName << " " << z_relative / cm << "\n";
          outMult.close();
        }
      }
    }
  }

  // --- Secondary production
  const auto* secondaries = step->GetSecondaryInCurrentStep();
  G4String creatorVolume = preVolume->GetName();

  // Tritium (triton) production — now in all volumes
  for (const auto* secondary : *secondaries) {
    if (secondary->GetDefinition()->GetParticleName() == "triton") {
      G4ThreeVector pos = secondary->GetPosition();
      G4double tritonEnergy = secondary->GetKineticEnergy();
      G4double z_relative = pos.z() - interfaceZ;

      G4cout << "[TRITON] Tritium produced in " << creatorVolume
             << " at " << pos / cm << " cm, E = " << tritonEnergy / MeV << " MeV" << G4endl;

      std::ofstream outTriton("triton_depth.txt", std::ios::app);
      outTriton << z_relative / cm << "\n";
      outTriton.close();

      fEventAction->AddTritium();
    }
  }

  // Helium (alpha) production — all materials
  for (const auto* secondary : *secondaries) {
    if (secondary->GetDefinition()->GetParticleName() == "alpha") {
      G4ThreeVector pos = secondary->GetPosition();
      G4double alphaEnergy = secondary->GetKineticEnergy();
      G4double z_relative = pos.z() - interfaceZ;

      G4cout << "[HELIUM] Alpha produced in " << creatorVolume
             << " at " << pos / cm << " cm, E = " << alphaEnergy / MeV << " MeV" << G4endl;

      std::ofstream outAlpha("alpha_depth.txt", std::ios::app);
      outAlpha << z_relative / cm << "\n";
      outAlpha.close();

      fEventAction->AddHelium();
    }
  }
}

}  // namespace B1
