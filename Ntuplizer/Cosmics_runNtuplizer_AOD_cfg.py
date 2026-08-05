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

# Source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        'file:/eos/user/a/aovergaa/SummerStudent_Project/CMSSW_15_0_5/src/CosmicMuons-FrameWork/Simulation/output/sim_v1/AODSIM.root'
        #'file:6ddf0cc8-31e6-4714-b3de-f521348e9b7f.root'
        #'file:14cbfe49-ca4a-4d65-aa56-d349ba6415a0.root', #RAW-RECO
        #'file:63cb89d6-f6e8-4ab8-b7df-178bad21884e.root', # annabela
        # 'file:/eos/cms/store/group/phys_muon/fernanpe/EventDisplays/46af13d3-f53a-4cb4-b640-b1606b065daa.root', #Commissioning 2025 file
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
