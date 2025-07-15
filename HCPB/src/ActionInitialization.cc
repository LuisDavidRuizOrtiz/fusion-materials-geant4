/// \file B1/src/ActionInitialization.cc
/// \brief Implementation of the B1::ActionInitialization class

#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"

namespace B1
{

void ActionInitialization::BuildForMaster() const
{
  // Only RunAction is needed for master thread
  RunAction* runAction = new RunAction();
  SetUserAction(runAction);
}

void ActionInitialization::Build() const
{
  // Access detector construction if needed (const-safe cast)
  const DetectorConstruction* detectorConstruction =
      static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

  // Create and register user actions
  auto* runAction    = new RunAction();
  auto* eventAction  = new EventAction(runAction);
  auto* genAction    = new PrimaryGeneratorAction();
  auto* stepAction   = new SteppingAction(eventAction);

  SetUserAction(genAction);
  SetUserAction(runAction);
  SetUserAction(eventAction);
  SetUserAction(stepAction);
}

}  // namespace B1
