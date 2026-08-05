import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import sys

# 1. Parser configuration
options = VarParsing.VarParsing('analysis')

# Remove default values (leave as empty strings '')
options.register('input',
                 '', 
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Input ROOT file")

options.register('output',
                 '', 
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Output ROOT file")

options.parseArguments()

# Safety check: Force the user to provide arguments via terminal
if not options.input or not options.output:
    print("\n[ERROR] Missing arguments. You MUST provide 'input' and 'output'.")
    print("Example: cmsRun Cosmics_runNtuplizer_AOD_cfg.py input=/path/to/input.root output=/path/to/output.root\n")
    sys.exit(1)

# 2. Differentiated path logic
clean_input = options.input.replace('file:', '')

# For input: Add 'file:' ONLY if it does NOT start with '/store/'
if not clean_input.startswith('/store/'):
    in_file = f'file:{clean_input}'
else:
    in_file = clean_input

# For output (TFileService): MUST NOT have file: prefix if it's a local/EOS path
out_file = options.output.replace('file:', '') 

print(f">>> Reading input from: {in_file}")
print(f">>> Writing output to: {out_file}")

process = cms.Process("MuonNtuple")

# Standard loads
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("TrackingTools.Configuration.TrackingTools_cff")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.GlobalTrackingGeometryESProducer = cms.ESProducer("GlobalTrackingGeometryESProducer")

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, "150X_dataRun3_Prompt_v1", "")

# Source (Input)
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(in_file)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)  
)

# Analyzer (Load the cfi FIRST)
process.load("CosmicMuons-FrameWork.Ntuplizer.muonNtupleProducer_cfi")

process.muonNtupleProducer.ServiceParameters = cms.PSet(
    Propagators=cms.untracked.vstring(
        "SteppingHelixPropagatorAny",
        "SteppingHelixPropagatorAlong",
        "SteppingHelixPropagatorOpposite",
    ),
    RPCLayers=cms.bool(True),
)

# TFileService (Output - Overwrite the output AFTER loading the cfi)
process.TFileService = cms.Service("TFileService", 
    fileName = cms.string(out_file)
)

process.p = cms.Path(process.muonNtupleProducer)
