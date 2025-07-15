/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Element.hh"
#include "G4Isotope.hh"

namespace B1
{

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4NistManager* nist = G4NistManager::Instance();

  // Updated envelope and plate sizes
  G4double plate_sizeXY = 200 * cm;
  G4double env_sizeXY = 1.05 * plate_sizeXY;  // 210 cm
  G4double env_sizeZ = 180 * cm;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_Galactic");

  G4bool checkOverlaps = true;

  // Updated world dimensions
  G4double world_sizeXY = 1.2 * env_sizeXY;  // 252 cm
  G4double world_sizeZ = 1.2 * env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");

  auto solidWorld = new G4Box("World", 0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);
  auto logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
  auto physWorld = new G4PVPlacement(nullptr, {}, logicWorld, "World", nullptr, false, 0, checkOverlaps);

  auto solidEnv = new G4Box("Envelope", 0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);
  auto logicEnv = new G4LogicalVolume(solidEnv, env_mat, "Envelope");
  new G4PVPlacement(nullptr, {}, logicEnv, "Envelope", logicWorld, false, 0, checkOverlaps);

  // --- Plate 1: Tungsten (W) — 2.0 cm 
  G4double plate1_sizeZ = 2.0 * cm; 
  G4Material* plate1_mat = nist->FindOrBuildMaterial("G4_W");
  G4ThreeVector pos1 = G4ThreeVector(0, 0, -41.0 * cm);  
  auto solidPlate1 = new G4Box("Plate1", 0.5 * plate_sizeXY, 0.5 * plate_sizeXY, 0.5 * plate1_sizeZ);
  auto logicPlate1 = new G4LogicalVolume(solidPlate1, plate1_mat, "Plate1");
  new G4PVPlacement(nullptr, pos1, logicPlate1, "Plate1", logicEnv, false, 0, checkOverlaps);
  logicPlate1->SetVisAttributes(new G4VisAttributes(G4Colour(0.1, 0.1, 0.1, 0.6)));

  // --- Plate 2: Enriched PbLi Alloy — 80 cm
  G4Element* Pb = new G4Element("Lead", "Pb", 82, 207.2 * g / mole);

  G4Isotope* Li6 = new G4Isotope("Li6", 3, 6, 6.015 * g / mole);
  G4Isotope* Li7 = new G4Isotope("Li7", 3, 7, 7.016 * g / mole);
  G4Element* enrichedLi = new G4Element("EnrichedLithium", "Li", 2);
  enrichedLi->AddIsotope(Li6, 90.*perCent);
  enrichedLi->AddIsotope(Li7, 10.*perCent);

  G4Material* li17pb83 = new G4Material("Li17Pb83", 9.4 * g/cm3, 2);
  li17pb83->AddElement(Pb, 0.9940);
  li17pb83->AddElement(enrichedLi, 0.0060);

  G4double plate2_sizeZ = 80 * cm;
  G4ThreeVector pos2 = G4ThreeVector(0, 0, 0);
  auto solidPlate2 = new G4Box("Plate2", 0.5 * plate_sizeXY, 0.5 * plate_sizeXY, 0.5 * plate2_sizeZ);
  auto logicPlate2 = new G4LogicalVolume(solidPlate2, li17pb83, "Plate2");
  new G4PVPlacement(nullptr, pos2, logicPlate2, "Plate2", logicEnv, false, 0, checkOverlaps);
  logicPlate2->SetVisAttributes(new G4VisAttributes(G4Colour(0.6, 0.2, 0.2, 0.7)));

  // --- Plate 3: EUROFER — 15 cm 
  G4Element* Fe = new G4Element("Iron", "Fe", 26, 55.845 * g / mole);
  G4Element* C  = new G4Element("Carbon", "C", 6, 12.011 * g / mole);
  G4Element* Mn = new G4Element("Manganese", "Mn", 25, 54.938 * g / mole);
  G4Element* Cr = new G4Element("Chromium", "Cr", 24, 52.00 * g / mole);
  G4Element* W  = new G4Element("Tungsten", "W", 74, 183.84 * g / mole);
  G4Element* V  = new G4Element("Vanadium", "V", 23, 50.94 * g / mole);

  G4Material* plate3_mat = new G4Material("EUROFER", 7.8 * g / cm3, 6);
  plate3_mat->AddElement(Fe, 0.8969);
  plate3_mat->AddElement(C,  0.0011);
  plate3_mat->AddElement(Mn, 0.0011);
  plate3_mat->AddElement(Cr, 0.09);
  plate3_mat->AddElement(W,  0.011);
  plate3_mat->AddElement(V,  0.0019);

  G4double plate3_sizeZ = 15 * cm;  
  G4ThreeVector pos3 = G4ThreeVector(0, 0, 47.5 * cm);
  auto solidPlate3 = new G4Box("Plate3", 0.5 * plate_sizeXY, 0.5 * plate_sizeXY, 0.5 * plate3_sizeZ);
  auto logicPlate3 = new G4LogicalVolume(solidPlate3, plate3_mat, "Plate3");
  new G4PVPlacement(nullptr, pos3, logicPlate3, "Plate3", logicEnv, false, 0, checkOverlaps);
  logicPlate3->SetVisAttributes(new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 0.5)));

  return physWorld;
}

}  // namespace B1
