# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: MultiCosmicGun_cfi --fileout file:GEN-SIM_MultiCosmic.root --mc --eventcontent RAWSIM --datatier GEN-SIM --conditions auto:phase1_2025_cosmics --beamspot NoVertexSmear --scenario cosmics --step GEN,SIM --geometry DB:Extended --era Run3 -n 100 --python_filename MultiCosmicGun_GEN_SIM_cfg.py --no_exec
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing #necessary for input options
import sys

from Configuration.Eras.Era_Run3_cff import Run3

# Shower settings
MaxZenithAngle = 1.047   # ~60 degrees 
ShowerHalfWidth = 0.04

# ----  Parser Configuration ---- #

options = VarParsing.VarParsing('analysis')

# define input and set no default values for nMuons
options.register('nMuons',
                 -1,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of muons generated per event")

options.register('nEvents',
                 1000,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of events to generate")

options.register('output',
                 'GEN-SIM_MultiCosmic.root',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Output GEN-SIM file")

options.parseArguments()

nGenMuons = options.nMuons

# force the user to provide non default argument via terminal
if nGenMuons < 0:
    print("\n[ERROR] Missing (valid) arguments. You MUST provide (positive values for) 'nMuons'. ")
    print("Example: cmsRun MultiCosmicGun_GEN_SIM_cfg.py nMuons={int} ")
    print("...")
    print("...")
    print("...")
    sys.exit(1)

# ensure local files are correctly referenced

out_file = options.output.replace('file:', '')

# ----  Print start message ---- #

print(f"... Generating {options.nEvents} events with {options.nMuons} muons/event")
print(f"... Writing output to: {out_file}")

# ---- Configure process ---- #

process = cms.Process('SIM',Run3)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContentCosmics_cff')
process.load('SimGeneral.MixingModule.mixCosmics_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.GeometrySimDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('Configuration.StandardSequences.VtxSmearedNoSmear_cff')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.SimNOBEAM_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


# Set events from input options
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.nEvents),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

# Input source
process.source = cms.Source("EmptySource")

process.options = cms.untracked.PSet(
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    TryToContinue = cms.untracked.vstring(),
    accelerators = cms.untracked.vstring('*'),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    deleteNonConsumedUnscheduledModules = cms.untracked.bool(True),
    dumpOptions = cms.untracked.bool(False),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(0)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    holdsReferencesToDeleteEarly = cms.untracked.VPSet(),
    makeTriggerResults = cms.obsolete.untracked.bool,
    modulesToCallForTryToContinue = cms.untracked.vstring(),
    modulesToIgnoreForDeleteEarly = cms.untracked.vstring(),
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(0),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('MultiCosmicGun_cfi nevts:100'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition
process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('generation_step')
    ),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string(f'file:{out_file}'),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
if hasattr(process, "XMLFromDBSource"): process.XMLFromDBSource.label="Extended"
if hasattr(process, "DDDetectorESProducerFromDB"): process.DDDetectorESProducerFromDB.label="Extended"
process.genstepfilter.triggerConditions=cms.vstring("generation_step")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2025_cosmics', '')

process.generator = cms.EDProducer("MultiVtxFlatRandomPtGunProducer",
    AddAntiParticle = cms.bool(False),
    PGunParameters = cms.PSet(
        MinEta = cms.double(-2.0),   # Dummy value for BaseFlatGunProducer, does not do anything
        MaxEta = cms.double(2.0),    # Same as above
        MaxZenithAngle = cms.double(MaxZenithAngle),
        ShowerHalfWidth = cms.double(ShowerHalfWidth),
        MaxPhi = cms.double(-1.58),
        MinPhi = cms.double(-1.56),
        MaxPt = cms.double(3000.0),
        MinPt = cms.double(100.0),
        PartID = cms.vint32([13]*nGenMuons)
    ),
    Verbosity = cms.untracked.int32(0)
)


process.ProductionFilterSequence = cms.Sequence(process.generator)

# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen)
process.simulation_step = cms.Path(process.psim)
process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.simulation_step,process.endjob_step,process.RAWSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)
# filter all path with the production filter sequence
for path in process.paths:
	getattr(process,path).insert(0, process.ProductionFilterSequence)

# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion

# =========================================================
# HACK: Move the origin of the muons to the top of the cavern
# =========================================================
process.VtxSmeared = cms.EDProducer("MultiVtxFlatEvtVtxGenerator",
    MinX = cms.double(-500.0), # Transversal area: 10 meters
    MaxX = cms.double(500.0),
    MinY = cms.double(800.0),  # Origin at 8 meter height (on top of the detector)
    MaxY = cms.double(800.0),
    # MinY = cms.double(13750.0),  # vertex high above detector to spread out muons. 137.50 meters should match a shower width of 5 degs.
    # MaxY = cms.double(13750.0),
    MinZ = cms.double(-600.0), # Longitudinal length: 12 meters
    MaxZ = cms.double(600.0),
    MinT = cms.double(0.0),
    MaxT = cms.double(0.0),
    # TimeOffset = cms.double(0.0), # TimeOffset is not read in MultiVtxFlatEvtGenerator, only original VtxFlatEvtGenerator
    src = cms.InputTag("generator", "unsmeared")
)
