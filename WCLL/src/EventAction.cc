#include "EventAction.hh"
#include "RunAction.hh"

#include <fstream>
#include <G4SystemOfUnits.hh>

namespace B1
{

EventAction::EventAction(RunAction* runAction)
  : fRunAction(runAction)
{}

void EventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.;

  fEnergiesBeforeW.clear();
  fEnergiesAfterW.clear();
  fEnergiesBeforeEUROFER.clear();
  fEnergiesAfterEUROFER.clear();

  fEdepByVolume.clear();
  fTritiumCount = 0;
  fHeliumCount = 0;

  fBackscattered = false;
  fNeutronInCount = 0;

  fEffectiveNeutron = false;  // ✅ Reset the flag for each event
}

void EventAction::EndOfEventAction(const G4Event*)
{
  // ✅ Always record physics quantities
  fRunAction->AddEdep(fEdep);
  fRunAction->AddTritium(fTritiumCount);
  fRunAction->AddHelium(fHeliumCount);

  for (const auto& [volumeName, edep] : fEdepByVolume) {
    fRunAction->AddEdepByVolume(volumeName, edep);
  }

  // ✅ Count effective neutrons (reached Plate2)
  if (fEffectiveNeutron) {
    fRunAction->AddEffectiveNeutrons(1);
  }

  // Output neutron energy spectra
  std::ofstream outBeforeW("neutrons_before_W.txt", std::ios::app);
  for (auto E : fEnergiesBeforeW)
    outBeforeW << E / MeV << "\n";
  outBeforeW.close();

  std::ofstream outAfterW("neutrons_after_W.txt", std::ios::app);
  for (auto E : fEnergiesAfterW)
    outAfterW << E / MeV << "\n";
  outAfterW.close();

  std::ofstream outBeforeEUROFER("neutrons_before_EUROFER.txt", std::ios::app);
  for (auto E : fEnergiesBeforeEUROFER)
    outBeforeEUROFER << E / MeV << "\n";
  outBeforeEUROFER.close();

  std::ofstream outAfterEUROFER("neutrons_after_EUROFER.txt", std::ios::app);
  for (auto E : fEnergiesAfterEUROFER)
    outAfterEUROFER << E / MeV << "\n";
  outAfterEUROFER.close();

  G4cout << "[TRITON] Tritium count this event: " << fTritiumCount << G4endl;
  G4cout << "[HELIUM] Helium (alpha) count this event: " << fHeliumCount << G4endl;
}

void EventAction::AddEnergyBeforeW(G4double energy)
{
  fEnergiesBeforeW.push_back(energy);
}

void EventAction::AddEnergyAfterW(G4double energy)
{
  fEnergiesAfterW.push_back(energy);
}

void EventAction::AddEnergyBeforeEUROFER(G4double energy)
{
  fEnergiesBeforeEUROFER.push_back(energy);
}

void EventAction::AddEnergyAfterEUROFER(G4double energy)
{
  fEnergiesAfterEUROFER.push_back(energy);
}

}  // namespace B1
