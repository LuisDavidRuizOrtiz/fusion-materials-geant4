#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>
#include <map>
#include <G4String.hh>

class G4Event;

namespace B1
{

class RunAction;

class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    void AddEdep(G4double edep) { fEdep += edep; }

    void AddEdepByVolume(const G4String& name, G4double edep) {
      fEdepByVolume[name] += edep;
    }
    const std::map<G4String, G4double>& GetEdepByVolume() const {
      return fEdepByVolume;
    }

    void AddEnergyBeforeW(G4double energy);
    void AddEnergyAfterW(G4double energy);
    void AddEnergyBeforeEUROFER(G4double energy);
    void AddEnergyAfterEUROFER(G4double energy);

    void AddTritium() { ++fTritiumCount; }
    int GetTritiumCount() const { return fTritiumCount; }

    void AddHelium() { ++fHeliumCount; }
    int GetHeliumCount() const { return fHeliumCount; }

    // Neutron backscatter tracking (optional)
    void MarkBackscattered() { fBackscattered = true; }
    bool IsBackscattered() const { return fBackscattered; }

    // Optional tracking: neutrons entering Plate1
    void IncrementNeutronInCount() { ++fNeutronInCount; }

    // Effective neutron logic (Be1-based)
    void MarkEffectiveNeutron() { fEffectiveNeutron = true; }
    bool IsEffectiveNeutron() const { return fEffectiveNeutron; }

  private:
    RunAction* fRunAction = nullptr;
    G4double fEdep = 0.;
    std::map<G4String, G4double> fEdepByVolume;

    std::vector<G4double> fEnergiesBeforeW;
    std::vector<G4double> fEnergiesAfterW;
    std::vector<G4double> fEnergiesBeforeEUROFER;
    std::vector<G4double> fEnergiesAfterEUROFER;

    int fTritiumCount = 0;
    int fHeliumCount = 0;

    int fNeutronInCount = 0;
    bool fBackscattered = false;

    // Flag for Be1 logic
    bool fEffectiveNeutron = false;
};

}  // namespace B1

#endif
