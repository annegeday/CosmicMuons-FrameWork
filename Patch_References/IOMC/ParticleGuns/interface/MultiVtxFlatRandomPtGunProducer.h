#ifndef MultiVtxFlatRandomPtGunProducer_H
#define MultiVtxFlatRandomPtGunProducer_H

/** \class MultiVtxFlatRandomPtGunProducer
 *
 * Generates single particle gun in HepMC format
 * Julia Yarba 12/2005 
 ***************************************/

#include "IOMC/ParticleGuns/interface/BaseFlatGunProducer.h"

namespace edm {

  class MultiVtxFlatRandomPtGunProducer : public BaseFlatGunProducer {
  public:
    MultiVtxFlatRandomPtGunProducer(const ParameterSet& pset);
    ~MultiVtxFlatRandomPtGunProducer() override;

    void produce(Event& e, const EventSetup& es) override;

  private:
    // data members
    double fMinPt;
    double fMaxPt;
    double fShowerHalfWidth;
    double fMaxZenithAngle;   // in radians, measured from vertical
  };
}  // namespace edm

#endif
