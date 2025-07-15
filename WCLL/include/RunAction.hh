#ifndef B1RunAction_h
#define B1RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"
#include <fstream>
#include <map>
#include <G4String.hh>

class G4Run;

namespace B1
{

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    ~RunAction() override;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    void AddEdep(G4double edep);
    void AddTritium(int count);
    void AddHelium(int count);

    void AddEdepByVolume(const G4String& name, G4double edep);

    // NEW: Add effective neutron count (for stats excluding backscatter)
    void AddEffectiveNeutrons(int count);

  private:
    G4Accumulable<G4double> fEdep = 0.;
    G4Accumulable<G4double> fEdep2 = 0.;
    G4Accumulable<int> fTritiumTotal = 0;
    G4Accumulable<int> fHeliumTotal = 0;
    G4Accumulable<int> fEffectiveNeutrons = 0; // NEW

    std::map<G4String, G4double> fLayerEdeps;

    std::ofstream outputFile;
};

}  // namespace B1

#endif
