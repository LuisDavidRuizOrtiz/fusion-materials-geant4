/// \file exampleB1.cc
/// \brief Main program of the B1 example

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "QBBC.hh"
#include "QGSP_BIC_HP.hh"

#include "G4RunManager.hh"
#include "G4SteppingVerbose.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

using namespace B1;

int main(int argc, char** argv)
{
  // Detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = nullptr;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Use G4SteppingVerboseWithUnits
  G4int precision = 4;
  G4SteppingVerbose::UseBestUnit(precision);

  // Construct the run manager (single-threaded)
  G4RunManager* runManager = new G4RunManager;

  // Set mandatory initialization classes
  runManager->SetUserInitialization(new DetectorConstruction());

  auto physicsList = new QGSP_BIC_HP;
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);

  runManager->SetUserInitialization(new ActionInitialization());

  // Initialize visualization
  auto visManager = new G4VisExecutive(argc, argv);
  visManager->Initialize();

  // Get the UI manager
  auto UImanager = G4UImanager::GetUIpointer();

  // Run in batch or interactive mode
  if (!ui) {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  } else {
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Clean up
  delete visManager;
  delete runManager;
}
