#ifndef IOMC_MultiVtxFlatEvtVtxGenerator_H
#define IOMC_MultiVtxFlatEvtVtxGenerator_H

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

namespace CLHEP {
  class HepRandomEngine;
}

namespace edm {
  class HepMCProduct;
}

class MultiVtxFlatEvtVtxGenerator : public edm::stream::EDProducer<> {
public:
  explicit MultiVtxFlatEvtVtxGenerator(const edm::ParameterSet&);
  ~MultiVtxFlatEvtVtxGenerator() override;

  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  edm::EDGetTokenT<edm::HepMCProduct> sourceToken_;

  double fMinX, fMinY, fMinZ, fMinT;
  double fMaxX, fMaxY, fMaxZ, fMaxT;
};

#endif
