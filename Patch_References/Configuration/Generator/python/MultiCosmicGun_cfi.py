import FWCore.ParameterSet.Config as cms
import math

generator = cms.EDProducer("FlatRandomPtGunProducer",
    PGunParameters = cms.PSet(
        # Muons per event defined in PartID                                                                                                                                                         
        PartID = cms.vint32(13, 13, 13, 13),
        MinPt  = cms.double(10.0),
        MaxPt  = cms.double(3000.0),
        MinEta = cms.double(-2.5),
        MaxEta = cms.double(2.5),
        # Negative phi: all the particles point downwards (-Y)                                                                                                                                      
        MinPhi = cms.double(-math.pi),
        MaxPhi = cms.double(0.0)
    ),
    AddAntiParticle = cms.bool(False),
    Verbosity       = cms.untracked.int32(0)
)

ProductionFilterSequence = cms.Sequence(generator)
