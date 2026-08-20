#include "IOMC/EventVertexGenerators/interface/MultiVtxFlatEvtVtxGenerator.h"
// #include <iostream> //add this for debugging
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"

#include <CLHEP/Random/RandFlat.h>
#include <CLHEP/Units/SystemOfUnits.h>
#include <CLHEP/Units/GlobalPhysicalConstants.h>

using CLHEP::cm;
using CLHEP::ns;
using namespace edm;

MultiVtxFlatEvtVtxGenerator::MultiVtxFlatEvtVtxGenerator(const ParameterSet& p) {
  fMinX = p.getParameter<double>("MinX") * cm;
  fMinY = p.getParameter<double>("MinY") * cm;
  fMinZ = p.getParameter<double>("MinZ") * cm;
  fMaxX = p.getParameter<double>("MaxX") * cm;
  fMaxY = p.getParameter<double>("MaxY") * cm;
  fMaxZ = p.getParameter<double>("MaxZ") * cm;
  fMinT = p.getParameter<double>("MinT") * ns * c_light;
  fMaxT = p.getParameter<double>("MaxT") * ns * c_light;

  if (fMinX > fMaxX)
    throw cms::Exception("Configuration") << "MultiVtxFlatEvtVtxGenerator: MinX is greater than MaxX";
  if (fMinY > fMaxY)
    throw cms::Exception("Configuration") << "MultiVtxFlatEvtVtxGenerator: MinY is greater than MaxY";
  if (fMinZ > fMaxZ)
    throw cms::Exception("Configuration") << "MultiVtxFlatEvtVtxGenerator: MinZ is greater than MaxZ";
  if (fMinT > fMaxT)
    throw cms::Exception("Configuration") << "MultiVtxFlatEvtVtxGenerator: MinT is greater than MaxT";

  sourceToken_ = consumes<edm::HepMCProduct>(p.getParameter<edm::InputTag>("src"));
  produces<edm::HepMCProduct>();
}

MultiVtxFlatEvtVtxGenerator::~MultiVtxFlatEvtVtxGenerator() {}

void MultiVtxFlatEvtVtxGenerator::produce(Event& evt, const EventSetup&) {
  edm::Service<edm::RandomNumberGenerator> rng;
  CLHEP::HepRandomEngine* engine = &rng->getEngine(evt.streamID());

  Handle<HepMCProduct> HepUnsmearedMCEvt;
  bool found = evt.getByToken(sourceToken_, HepUnsmearedMCEvt);
  if (!found)
    throw cms::Exception("ProductAbsent") << "MultiVtxFlatEvtVtxGenerator: no HepMCProduct found.";

  // deep copy so each vertex can be repositioned independently
  HepMC::GenEvent* genevt = new HepMC::GenEvent(*HepUnsmearedMCEvt->GetEvent());

  for (auto vtxIt = genevt->vertices_begin(); vtxIt != genevt->vertices_end(); ++vtxIt) {
    double aX = CLHEP::RandFlat::shoot(engine, fMinX, fMaxX);
    double aY = CLHEP::RandFlat::shoot(engine, fMinY, fMaxY);
    double aZ = CLHEP::RandFlat::shoot(engine, fMinZ, fMaxZ);
    double aT = CLHEP::RandFlat::shoot(engine, fMinT, fMaxT);
    (*vtxIt)->set_position(HepMC::FourVector(aX, aY, aZ, aT));

    //std::cout << "Vertex barcode " << (*vtxIt)->barcode() 						//add these two lines for debugging
           //<< " placed at [" << aX << ", " << aY << ", " << aZ << ", " << aT << "]" << std::endl;
}

  std::unique_ptr<edm::HepMCProduct> HepMCEvt(new edm::HepMCProduct(genevt));
  evt.put(std::move(HepMCEvt));
}
