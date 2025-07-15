/// \file B1/include/SteppingAction.hh
/// \brief Definition of the B1::SteppingAction class

#ifndef SteppingAction_h
#define SteppingAction_h

#include "G4UserSteppingAction.hh"
#include "G4SystemOfUnits.hh"

namespace B1
{

class EventAction;

/// Stepping action class

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction* eventAction);
    ~SteppingAction() override;

    void UserSteppingAction(const G4Step* step) override;

  private:
    EventAction* fEventAction;
};

}  // namespace B1

#endif
