import FWCore.ParameterSet.Config as cms

process = cms.Process("MuonNtuple")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("TrackingTools.Configuration.TrackingTools_cff")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.GlobalTrackingGeometryESProducer = cms.ESProducer("GlobalTrackingGeometryESProducer")

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, "150X_dataRun3_Prompt_v1", "") # Crucial to get the magnetic field properly

# Set input file path
inputPath = 'file:/eos/cms/store/group/phys_muon/fernanpe/EventDisplays/46af13d3-f53a-4cb4-b640-b1606b065daa.root'

# Source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        inputPath,
    )
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

#output
process.TFileService = cms.Service("TFileService", fileName=cms.string("file:ntuples.root"))

# Analyzer
process.load("CosmicMuons-FrameWork.Ntuplizer.muonNtupleProducer_cfi")


process.muonNtupleProducer.ServiceParameters = cms.PSet(
    Propagators=cms.untracked.vstring(
        "SteppingHelixPropagatorAny",
        "SteppingHelixPropagatorAlong",
        "SteppingHelixPropagatorOpposite",
    ),
    RPCLayers=cms.bool(True),
)

process.p = cms.Path(process.muonNtupleProducer)
