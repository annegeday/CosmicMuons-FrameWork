/*
 *  \author Julia Yarba
 */

#include <ostream>
//#include <iostream> //For debugging


#include "IOMC/ParticleGuns/interface/MultiVtxFlatRandomPtGunProducer.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"

#include "CLHEP/Random/RandFlat.h"

using namespace edm;
using namespace std;

MultiVtxFlatRandomPtGunProducer::MultiVtxFlatRandomPtGunProducer(const ParameterSet& pset) : BaseFlatGunProducer(pset) {
  ParameterSet defpset;
  ParameterSet pgun_params = pset.getParameter<ParameterSet>("PGunParameters");

  fMinPt = pgun_params.getParameter<double>("MinPt");
  fMaxPt = pgun_params.getParameter<double>("MaxPt");
  fShowerHalfWidth = pgun_params.getParameter<double>("ShowerHalfWidth");
  fMaxZenithAngle = pgun_params.getParameter<double>("MaxZenithAngle");


  produces<HepMCProduct>("unsmeared");
  produces<GenEventInfoProduct>();
}

MultiVtxFlatRandomPtGunProducer::~MultiVtxFlatRandomPtGunProducer() {
  // no need to cleanup GenEvent memory - done in HepMCProduct
}

void MultiVtxFlatRandomPtGunProducer::produce(Event& e, const EventSetup& es) {
  edm::Service<edm::RandomNumberGenerator> rng;
  CLHEP::HepRandomEngine* engine = &rng->getEngine(e.streamID());

  if (fVerbosity > 0) {
    cout << "MultiVtxFlatRandomPtGunProducer : Begin New Event Generation" << endl;
  }
  // event loop (well, another step in it...)

  // no need to clean up GenEvent memory - done in HepMCProduct
  //

  // here re-create fEvt (memory)
  //
  fEvt = new HepMC::GenEvent();

  // draw this event's shower direction from a cos^2(zenith angle) distribution,
  // symmetric about vertical (alpha = 0)
  double alpha;
  while (true) {
    alpha = CLHEP::RandFlat::shoot(engine, 0., fMaxZenithAngle);
    double u = CLHEP::RandFlat::shoot(engine, 0., 1.);
    if (u < cos(alpha) * cos(alpha))
      break;
  }
  if (CLHEP::RandFlat::shoot(engine, 0., 1.) < 0.5)
    alpha = -alpha;   // equally likely to tilt toward +z or -z

  double thetaMid = M_PI / 2. - alpha;
  double etaMid = -log(tan(thetaMid / 2.));

  //std::cout << "Event zenith alpha = " << alpha * 180. / M_PI << " deg, etaMid = " << etaMid << std::endl; //for debugging

  double evtMinEta = etaMid - fShowerHalfWidth;
  double evtMaxEta = etaMid + fShowerHalfWidth;


  // now actualy, cook up the event from PDGTable and gun parameters
  //
  // 1st, primary vertex
  //
  //HepMC::GenVertex* Vtx = new HepMC::GenVertex(HepMC::FourVector(0., 0., 0.));
  //^this is commented out and moved to loop in order to generate new vertex per particle in same event
  // loop over particles
  //
  int barcode = 1;
  for (unsigned int ip = 0; ip < fPartIDs.size(); ++ip) {
    HepMC::GenVertex* Vtx = new HepMC::GenVertex(HepMC::FourVector(0., 0., 0.));
    double pt = CLHEP::RandFlat::shoot(engine, fMinPt, fMaxPt);
    double eta = CLHEP::RandFlat::shoot(engine, evtMinEta, evtMaxEta);
    double phi = CLHEP::RandFlat::shoot(engine, fMinPhi, fMaxPhi);
    int PartID = fPartIDs[ip];
    const HepPDT::ParticleData* PData = fPDGTable->particle(HepPDT::ParticleID(abs(PartID)));
    double mass = PData->mass().value();
    double theta = 2. * atan(exp(-eta));
    double mom = pt / sin(theta);
    double px = pt * cos(phi);
    double py = pt * sin(phi);
    double pz = mom * cos(theta);
    double energy2 = mom * mom + mass * mass;
    double energy = sqrt(energy2);
    HepMC::FourVector p(px, py, pz, energy);
    HepMC::GenParticle* Part = new HepMC::GenParticle(p, PartID, 1);
    Part->suggest_barcode(barcode);
    barcode++;
    Vtx->add_particle_out(Part);

    if (fAddAntiParticle) {
      HepMC::FourVector ap(-px, -py, -pz, energy);
      int APartID = -PartID;
      if (PartID == 22 || PartID == 23) {
        APartID = PartID;
      }
      HepMC::GenParticle* APart = new HepMC::GenParticle(ap, APartID, 1);
      APart->suggest_barcode(barcode);
      barcode++;
      Vtx->add_particle_out(APart);
    }
    fEvt->add_vertex(Vtx);
  }

  fEvt->set_event_number(e.id().event());
  fEvt->set_signal_process_id(20);

  if (fVerbosity > 0) {
    fEvt->print();
  }

  unique_ptr<HepMCProduct> BProduct(new HepMCProduct());
  BProduct->addHepMCData(fEvt);
  e.put(std::move(BProduct), "unsmeared");

  unique_ptr<GenEventInfoProduct> genEventInfo(new GenEventInfoProduct(fEvt));
  e.put(std::move(genEventInfo));

  if (fVerbosity > 0) {
    // for testing purpose only
    // fEvt->print() ; // prints empty info after it's made into edm::Event
    cout << "MultiVtxFlatRandomPtGunProducer : Event Generation Done " << endl;
  }
}
//#include "FWCore/Framework/interface/MakerMacros.h"
//DEFINE_FWK_MODULE(MultiVtxFlatRandomPtGunProducer);
