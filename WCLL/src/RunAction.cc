/// \file B1/src/RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"

#include "G4AccumulableManager.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <fstream>
#include <map>

namespace B1
{

RunAction::RunAction()
{
  // Open the output file
  outputFile.open("neutron_spectrum.txt");
  if (!outputFile.is_open()) {
    G4cerr << "Error opening the output file!" << G4endl;
    exit(1);
  }

  // Define custom units for dose (optional)
  new G4UnitDefinition("milligray", "milliGy", "Dose", 1.e-3 * gray);
  new G4UnitDefinition("microgray", "microGy", "Dose", 1.e-6 * gray);
  new G4UnitDefinition("nanogray", "nanoGy", "Dose", 1.e-9 * gray);
  new G4UnitDefinition("picogray", "picoGy", "Dose", 1.e-12 * gray);

  // Register accumulables
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Register(fEdep);
  accumulableManager->Register(fEdep2);
  accumulableManager->Register(fTritiumTotal);
  accumulableManager->Register(fHeliumTotal);
  accumulableManager->Register(fEffectiveNeutrons); 
}

RunAction::~RunAction()
{
  if (outputFile.is_open()) {
    outputFile.close();
  }
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  G4AccumulableManager::Instance()->Reset();
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  G4double edep = fEdep.GetValue();
  G4double edep2 = fEdep2.GetValue();
  G4int totalTritium = fTritiumTotal.GetValue();
  G4int totalHelium = fHeliumTotal.GetValue();
  G4int totalEffectiveNeutrons = fEffectiveNeutrons.GetValue();

  G4double rms = edep2 - edep * edep / nofEvents;
  rms = (rms > 0.) ? std::sqrt(rms) : 0.;

  // Output summary
  if (IsMaster()) {
    outputFile << "-------------------- End of Global Run ---------------------\n";
  } else {
    outputFile << "-------------------- End of Local Run ----------------------\n";
  }

  outputFile << "The run consists of " << nofEvents << " events.\n"
             << "Total energy deposited: " << G4BestUnit(edep, "Energy") << "\n"
             << "RMS: " << G4BestUnit(rms, "Energy") << "\n"
             << "Total tritium nuclei produced: " << totalTritium << "\n"
             << "Total helium nuclei produced: " << totalHelium << "\n"
             << "Total effective neutrons (non-backscattered): " << totalEffectiveNeutrons << "\n";

  // Output per-layer energy deposition
  outputFile << "\n--- Energy deposition by layer ---\n";

  // Define hardcoded masses for each plate
  std::map<G4String, G4double> layerMass_kg = {
    { "Plate1", 10.125 },
    { "Plate2", 678.24 },
    { "Plate3", 63.18 }
  };

  for (const auto& [vol, edepVol] : fLayerEdeps) {
    outputFile << "Layer: " << vol
               << ", Energy deposited: " << G4BestUnit(edepVol, "Energy");

    auto it = layerMass_kg.find(vol);
    if (it != layerMass_kg.end()) {
      G4double dose = edepVol / joule / it->second * gray;
      outputFile << ", Absorbed dose: " << G4BestUnit(dose, "Dose");
    } else {
      outputFile << ", Absorbed dose: N/A (mass unknown)";
    }

    outputFile << "\n";
  }

  outputFile << "------------------------------------------------------------\n\n";
}

void RunAction::AddEdep(G4double edep)
{
  fEdep += edep;
  fEdep2 += edep * edep;
}

void RunAction::AddTritium(int count)
{
  fTritiumTotal += count;
}

void RunAction::AddHelium(int count)
{
  fHeliumTotal += count;
}

void RunAction::AddEdepByVolume(const G4String& name, G4double edep)
{
  fLayerEdeps[name] += edep;
}

// NEW: Add effective neutron count
void RunAction::AddEffectiveNeutrons(int count)
{
  fEffectiveNeutrons += count;
}

}  // namespace B1
