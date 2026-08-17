import FWCore.ParameterSet.Config as cms

muonNtupleProducer = cms.EDAnalyzer("MuonNtupleProducer",
    muons = cms.InputTag("muons1Leg"),
    segmentsDt = cms.InputTag('dt4DSegments'),
    segmentsCSC = cms.InputTag('cscSegments'),
    genParticleCollection = cms.InputTag("genParticles"),
    recHitsDt = cms.InputTag('dt1DRecHits'),
    recHitsCSC = cms.InputTag('csc2DRecHits')
)
