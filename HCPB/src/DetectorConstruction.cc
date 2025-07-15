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

  // --- Updated envelope and plate sizes ---
  G4double plate_sizeXY = 200 * cm;
  G4double env_sizeXY = 1.05 * plate_sizeXY;  // 210 cm
  G4double env_sizeZ = 110 * cm;

  // Use high vacuum instead of air
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_Galactic");

  G4bool checkOverlaps = true;

  // Updated world dimensions
  G4double world_sizeXY = 1.2 * env_sizeXY;  // 252 cm
  G4double world_sizeZ = 1.2 * env_sizeZ;

  auto solidWorld = new G4Box("World", 0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);
  auto logicWorld = new G4LogicalVolume(solidWorld, env_mat, "World");
  auto physWorld = new G4PVPlacement(nullptr, {}, logicWorld, "World", nullptr, false, 0, checkOverlaps);

  auto solidEnv = new G4Box("Envelope", 0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);
  auto logicEnv = new G4LogicalVolume(solidEnv, env_mat, "Envelope");
  new G4PVPlacement(nullptr, {}, logicEnv, "Envelope", logicWorld, false, 0, checkOverlaps);

  // --- Plate 1: Tungsten (W) — 2.0 cm
  G4double plate1_sizeZ = 2.0 * cm;
  G4Material* plate1_mat = nist->FindOrBuildMaterial("G4_W");
  G4double wallOffset = 3.0 * cm;
  G4ThreeVector pos1 = G4ThreeVector(0, 0, -0.5 * env_sizeZ + wallOffset + 0.5 * plate1_sizeZ);
  auto solidPlate1 = new G4Box("Plate1", 0.5 * plate_sizeXY, 0.5 * plate_sizeXY, 0.5 * plate1_sizeZ);
  auto logicPlate1 = new G4LogicalVolume(solidPlate1, plate1_mat, "Plate1");
  new G4PVPlacement(nullptr, pos1, logicPlate1, "Plate1", logicEnv, false, 0, checkOverlaps);
  logicPlate1->SetVisAttributes(new G4VisAttributes(G4Colour(0.1, 0.1, 0.1, 0.6)));

  // --- Materials: Be and 90% Li6-enriched Li2TiO3
  G4Material* be_mat = nist->FindOrBuildMaterial("G4_Be");

  G4Isotope* Li6 = new G4Isotope("Li6", 3, 6, 6.015 * g/mole);
  G4Isotope* Li7 = new G4Isotope("Li7", 3, 7, 7.016 * g/mole);
  G4Element* Li = new G4Element("EnrichedLithium", "Li", 2);
  Li->AddIsotope(Li6, 90.*perCent);
  Li->AddIsotope(Li7, 10.*perCent);

  G4Element* Ti = new G4Element("Titanium", "Ti", 22, 47.867 * g/mole);
  G4Element* O  = new G4Element("Oxygen", "O", 8, 16.00 * g/mole);

  G4Material* li2tio3 = new G4Material("Li2TiO3", 3.1 * g/cm3, 3);
  li2tio3->AddElement(Li, 2);
  li2tio3->AddElement(Ti, 1);
  li2tio3->AddElement(O, 3);

  // --- Plate 2: Breeder container (80 cm)
  G4double breeder_sizeZ = 80 * cm;
  auto solidPlate2 = new G4Box("Plate2", 0.5 * plate_sizeXY, 0.5 * plate_sizeXY, 0.5 * breeder_sizeZ);
  auto logicPlate2 = new G4LogicalVolume(solidPlate2, env_mat, "Plate2");
  G4ThreeVector pos2 = G4ThreeVector(0, 0, pos1.z() + 0.5 * plate1_sizeZ + 0.5 * breeder_sizeZ);
  new G4PVPlacement(nullptr, pos2, logicPlate2, "Plate2", logicEnv, false, 0, checkOverlaps);
  logicPlate2->SetVisAttributes(new G4VisAttributes(G4Colour(0.8, 0.8, 0.8, 0.1)));

  // --- Interleaved breeder layers
  G4double be_thickness = 6.4 * cm;
  G4double li_thickness = 9.6 * cm;
  G4double z_cursor = -0.5 * breeder_sizeZ;

  for (int i = 0; i < 10; ++i) {
    G4bool isBe = (i % 2 == 0);
    G4Material* mat = isBe ? be_mat : li2tio3;
    G4double thick = isBe ? be_thickness : li_thickness;
    G4String name = isBe ? "Be" + std::to_string(i / 2 + 1)
                         : "Li2TiO3_" + std::to_string(i / 2 + 1);

    auto solidLayer = new G4Box(name, 0.5 * plate_sizeXY, 0.5 * plate_sizeXY, 0.5 * thick);
    auto logicLayer = new G4LogicalVolume(solidLayer, mat, name);
    G4ThreeVector rel_pos(0, 0, z_cursor + 0.5 * thick);
    new G4PVPlacement(nullptr, rel_pos, logicLayer, name, logicPlate2, false, i + 1, checkOverlaps);

    logicLayer->SetVisAttributes(new G4VisAttributes(
      isBe ? G4Colour(0.7, 0.7, 0.3, 0.4) : G4Colour(0.2, 0.6, 1.0, 0.5)));

    z_cursor += thick;
  }

  // --- Plate 3: EUROFER — 15 cm
  G4Element* Fe = new G4Element("Iron", "Fe", 26, 55.845 * g/mole);
  G4Element* C  = new G4Element("Carbon", "C", 6, 12.011 * g/mole);
  G4Element* Mn = new G4Element("Manganese", "Mn", 25, 54.938 * g/mole);
  G4Element* Cr = new G4Element("Chromium", "Cr", 24, 52.00 * g/mole);
  G4Element* W  = new G4Element("Tungsten", "W", 74, 183.84 * g/mole);
  G4Element* V  = new G4Element("Vanadium", "V", 23, 50.94 * g/mole);

  G4Material* plate3_mat = new G4Material("EUROFER", 7.8 * g/cm3, 6);
  plate3_mat->AddElement(Fe, 0.8969);
  plate3_mat->AddElement(C,  0.0011);
  plate3_mat->AddElement(Mn, 0.0011);
  plate3_mat->AddElement(Cr, 0.088);
  plate3_mat->AddElement(W,  0.011);
  plate3_mat->AddElement(V,  0.0019);

  G4double plate3_sizeZ = 15 * cm;
  G4ThreeVector pos3 = G4ThreeVector(0, 0, pos2.z() + 0.5 * breeder_sizeZ + 0.5 * plate3_sizeZ);
  auto solidPlate3 = new G4Box("Plate3", 0.5 * plate_sizeXY, 0.5 * plate_sizeXY, 0.5 * plate3_sizeZ);
  auto logicPlate3 = new G4LogicalVolume(solidPlate3, plate3_mat, "Plate3");
  new G4PVPlacement(nullptr, pos3, logicPlate3, "Plate3", logicEnv, false, 0, checkOverlaps);
  logicPlate3->SetVisAttributes(new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 0.5)));

  return physWorld;
}

}  // namespace B1
