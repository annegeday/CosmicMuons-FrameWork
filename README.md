# Cosmics Muons Ntupler

## Instructions for installing 

### Installing

    cmsrel CMSSW_15_0_5
    cd CMSSW_15_0_5/src
    cmsenv
    git clone git@github.com:fmanteca/CosmicMuons-FrameWork.git
    scram b -j 8

### Producing Ntuples
The framework will produce Ntuples from AOD. Every time you make a change anywhere, for instance in 'plugins/MuonNtupleProducer.cc', do not forget to re-compile (run 'scram b -j 8' in 'CMSSW_15_0_5/src').

    cd CosmicMuons-FrameWork/Ntuplizer/test/
        
    # Find an input AOD file and use it on process.source in 'Cosmics_runNtuplizer_AOD_cfg.py':
    # Use DAS to get the paths: https://cmsweb.cern.ch/das/request?view=list&limit=50&instance=prod%2Fglobal&input=dataset%3D%2F*Cosmics*%2FRun*2025*%2FAOD
    # One can either give the path of any of them, i.e., fileNames = cms.untracked.vstring('/store/data/Commissioning2025/Cosmics/AOD/PromptReco-v1/000/389/353/00000/896a0637-7186-4d8e-8af5-b11e22c90ea4.root')
    # or copy it in local 'xrdcp root://cms-xrd-global.cern.ch//store/data/Commissioning2025/Cosmics/AOD/PromptReco-v1/000/389/353/00000/896a0637-7186-4d8e-8af5-b11e22c90ea4.root ./'
    # and then point to it: 'fileNames = cms.untracked.vstring('file:896a0637-7186-4d8e-8af5-b11e22c90ea4.root')'. The latter option should be faster for testing purposes.
    
    cmsRun Cosmics_runNtuplizer_AOD_cfg.py

### Submit jobs to HTCondor

Before running the following commands, set your personal path in run.sh and select the input datasets and output path in prepare_files.py.

prepare_files.py will produce a txt file where each row will contain a pair input_file output_file. 

condor.sub will submit one job per row to the cluster, taking run.sh as the executable.

    cd condor
    voms-proxy-init --voms cms --hours 96  -out ${HOME}/.x509up_${UID};export X509_USER_PROXY=${HOME}/.x509up_${UID}
    python3 prepare_files.py
    condor_submit condor.sub

Merge the outputs with hadd. Example:

    hadd /eos/cms/store/group/phys_muon/fernanpe/Cosmics2025/merged.root /eos/cms/store/group/phys_muon/fernanpe/Cosmics2025/*/*root
    
### OR Submit jobs to crab
The following instructions will allow to submit jobs to the cluster for an entire dataset like '/Cosmics/Run2025A-PromptReco-v1/AOD'. The output ntuples can be stored in your '/eos/user/' area if T3_CH_CERNBOX is set as storage site (see crab_CosmicsData_2025A_AOD.py).

    source /cvmfs/cms.cern.ch/crab3/crab.sh
    cmsenv
    voms-proxy-init --voms cms --valid 168:00
    crab submit crab_Commissioning2025.py

This option is useful when the dataset is no longer available on disk, as crab will request a copy on disk automatically before starting running the jobs.

### Useful links:
* Muon reconstruction documentation: https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuons
* Cosmic muon reconstruction documentation: https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideCosmicMuonReco
* reco::Muon class: https://github.com/cms-sw/cmssw/blob/master/DataFormats/MuonReco/interface/Muon.h
* Muon POG selections & definitions: https://github.com/cms-sw/cmssw/blob/master/DataFormats/MuonReco/src/MuonSelectors.cc
* OMS (used to get run numbers from CRUZET/CRAFT): https://cmsoms.cern.ch/cms/run_3_cruz/cruzet_2025?cms_run_sequence=GLOBAL-RUN
* Get the datasets containing a given run number in DAS: https://cmsweb.cern.ch/das/request?view=list&limit=50&instance=prod%2Fglobal&input=dataset+run%3D389767

### Event displays with Fireworks (accept miniAOD/AOD/RECO formats as input)

Use edmPickEvents.py to filter out events if needed (see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookPickEvents).

Copy the output to a public /eos/cms/store/group/ path.

Insert path here: [https://fireworks.cern.ch/cmsShowWeb/revetor.pl](https://fireworks.cern.ch/cmsShowWeb/service.pl) (starting with /store/)

There is also the possibility to read files from your /eos/user/ private path. Follow the instructions here: https://github.com/alja/FireworksWeb/blob/main/doc/UserGuide.md

Instructions for cosmic multi-muons:
* Add Collections -> type "leg" -> select "Muons muon1Leg"
* FilterDialog -> $Muons10_muons1Leg@.size()>10 
* 3D -> i -> Geometry -> show all

### Including muon segments and hits in AOD

It turns out that, by default, CMS saves muon segments in AOD for pp collision runs, but not for cosmics. One has to add them here: https://github.com/cms-sw/cmssw/blob/master/RecoLocalMuon/Configuration/python/RecoLocalMuonCosmics_EventContent_cff.py#L5. Benchmark from pp cfg: https://github.com/cms-sw/cmssw/blob/master/RecoLocalMuon/Configuration/python/RecoLocalMuon_EventContent_cff.py#L7-L13.

Follow these instructions to produce customized AOD (with muon segments in) from RAW data:

    git cms-addpkg RecoLocalMuon/Configuration
    Edit RecoLocalMuon/Configuration/python/RecoLocalMuonCosmics_EventContent_cff.py
    scram b -j 20
    cmsDriver.py CosmicPPreco --step RAW2DIGI,RECO --datatier AOD --eventcontent AOD --filein=/store/data/Run2024C/Cosmics/RAW/v1/000/379/417/00000/022b1b63-e126-4800-be9a-cbd752664a95.root --fileout file:CosmicPPreco_RAW2DIGI_RECO.root --conditions 140X_dataRun3_Prompt_v2 --era Run3 --scenario cosmics --data -n 100

Finally, submit jobs to crab taking /Cosmics/Commissioning2025-v1/RAW as the input dataset:

     crab submit  crab_CosmicPPreco.py

## Simulation studies

We will make use of the `FlatRandomPtGunProducer` to generate guns of cosmic muons. It is designed to simulate collisions, meaning it generates particles exactly at the center of the detector (0,0,0) and fires them outwards. If one uses the module as is, the muons will be born at the center. Half will go upwards (passing only through the top half) and half downwards (passing only through the bottom half). To get them to completely pass through the detector from top to bottom, simulating cosmic rays, we have to trick CMSSW by doing two things: 

1. Restricting the angles so that the "Gun" only fires downwards.

2. Move the collision vertex to the top of the detector cavern.

#### 1. The Generator: Firing Downwards.

In the CMS coordinate system, the Y-axis points upwards. The azimuthal angle $\phi$ dictates the direction in the transverse plane: $\phi = +\pi/2$ is upwards. $\phi = -\pi/2$ is directly downwards. We configure the generator to fire, for example, 4 muons that go exclusively in the lower hemisphere (downwards):

     git cms-init
     git cms-addpkg Configuration/Generator
     cd Configuration/Generator/python
     cmsenv
     
Create a file `MultiCosmicGun_cfi.py` in `Configuration/Generator/python` with the following content:
     
```
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
        # Nehative phi: all the particles point downwards (-Y)                                                                                                                                      
        MinPhi = cms.double(-math.pi),
        MaxPhi = cms.double(0.0)
    ),
    AddAntiParticle = cms.bool(False),
    Verbosity       = cms.untracked.int32(0)
)

ProductionFilterSequence = cms.Sequence(generator)
```

And create the CMSSW config file to produce GEN-SIM under Run-3 cosmic conditions:

     (cd CMSSW_X_Y/src)
     cmsDriver.py MultiCosmicGun_cfi      --fileout file:GEN-SIM_MultiCosmic.root      --mc      --eventcontent RAWSIM      --datatier GEN-SIM      --conditions auto:phase1_2025_cosmics      --beamspot NoVertexSmear      --scenario cosmics      --step GEN,SIM      --geometry DB:Extended      --era Run3      -n 100      --python_filename MultiCosmicGun_GEN_SIM_cfg.py      --no_exec

#### 2. The Vertex: Move the origin to the top.

Now we need to tell CMSSW that these particles are not born at the center $(0,0,0)$, but on an imaginary plane above the detector (for example, at $Y = +800$ cm, just outside the muon barrel). Add this block at the end of the produced cfg file:

```
# =========================================================
# HACK: Move the origin of the muons to the top of the cavern
# =========================================================
process.VtxSmeared = cms.EDProducer("FlatEvtVtxGenerator",
    MinX = cms.double(-500.0), # Transversal area: 10 meters 
    MaxX = cms.double(500.0),
    MinY = cms.double(800.0),  # Origin at 8 meter height (on top of the detector)
    MaxY = cms.double(800.0),
    MinZ = cms.double(-600.0), # Longitudinal length: 12 meters 
    MaxZ = cms.double(600.0),
    MinT = cms.double(0.0),
    MaxT = cms.double(0.0),
    TimeOffset = cms.double(0.0),
    src = cms.InputTag("generator", "unsmeared")
)
```

Finally, run it:

     cmsRun MultiCosmicGun_GEN_SIM_cfg.py

The output file should contain GEN-SIM information. You can check out the event content by running:

     edmDumpEventContent GEN-SIM_MultiCosmic.root

#### 3. Produce AOD

The last step consists of running the GEN-SIM -> AOD step, so that we have the same data format as in data, but including information from the generated particles.

     cmsDriver.py step2      --filein file:GEN-SIM_MultiCosmic.root      --fileout file:AODSIM.root      --mc      --eventcontent AOD      --datatier AOD      --conditions auto:phase1_2025_cosmics      --scenario cosmics      --step DIGI,L1,DIGI2RAW,HLT,RAW2DIGI,L1Reco,RECO,RECOSIM      --geometry DB:Extended      --era Run3      -n -1      --python_filename GEN_SIM_to_AOD_cfg.py      --no_exec

Unfortunately, there is not a dedicated `--condition` for Run3 cosmics MC so far. The cmsDriver command above takes the conditions from data, so a couple of modifitions are needed to ensure that the generated information is stored in the AOD output.

First, comment out the following line in `GEN_SIM_to_AOD_cfg.py`: `'drop *_genParticles_*_*',`.

Second, add these lines in `RecoLocalMuonAOD` within `RecoLocalMuon/Configuration/python/RecoLocalMuonCosmics_EventContent_cff.py` as done in section "Including muon segments and hits in AOD"

```
'keep *_genParticles_*_*',
'keep *_generator_*_*',
'keep *_g4SimHits_Muon*_*',
```

By doing this, the generated particle kinematics, generator information, and the simulated muon hits will be stored in the AOD output.

Compile from CMSSW_X_Y/src and run:

     scram b -j 20
     cmsRun GEN_SIM_to_AOD_cfg.py

Now one can feed FireWorks with the AOD output file, make the flat ntuples, etc. 

Note that some modifications will be needed to include the generated particles truth information in the flat ntuples, i.e., read the `genParticles` collection, loop over the elements, store in new output branches the kinematic information from the muons. This is a good exercise for homework. In case it helps, I used to read that collection in the past for another project. The code logic behind is different as I was geometrically matching gen and reco muons, but some parts of the code can be taken as a benchmark: https://github.com/fmanteca/HighPt_DNN/tree/master/MyAnalysis/RECOAnalysis

## Directory structure and select file descriptions (WIP)
DataSegment_AOD/
* CosmicPPreco_RAW2DIGI_RECO.py:
     --> Takes raw cosmic data files and generates AOD files including segment data.
     --> Input params
              FileString: Path to raw data file in string format (Don't forget file: for a local file).
              MaxEvts: Number of events from the selected file(s), that you'd like to process.
              numCore: Number of cores allocated for job. Standard is 1 (2 for a crab job).
     --> Output: AOD file with name "CosmicPPreco_RAW2DIGI_RECO.root" in working directory. 
* crab_CosmicPPreco.py:
     --> Submits job to crab in order to process many data files.
     --> Selected input params
              config.JobType.numCores: **Must** match numCores in CosmicPPreco_RAW2DIGI_RECO.py.
              config.JobType.maxMemoryMB: Memory allocated, depends on number of cores (1 core: 2500-3000)
              config.Data.outLFNDirBase: Placement of output directory
              config.Data.outputDatasetTag: Name of output directory
              config.Data.inputDataset: DAS dataset name
     --> Output: AOD files with names "CosmicPPreco_RAW2DIGI_RECO_{JOB-NUMBER}.root" in generated output directory.

Simulation/
* MultiCosmicGun_GEN_SIM_cfg.py:
     --> Simulates cosmic muon showers by generating them above the detector with momentum going towards the detector. Each muon is generated with a random origin along the  axis parallel to the beamline. The direction of the shower is determined by Max/Min eta.  
     --> Outputs GEN-SIM_MultiCosmic.root
* GEN_SIM_to_AOD_cfg.py
     --> Takes simulated muons and generates "data-like" AOD files.
     --> Outputs AODSIM.root

Ntuplizer/
* test/Cosmics_runNtuplizer_AOD_cfg.py
     --> Produces Ntuples including segment info. If the data comes from simulated muons, the output will contain info regarding the muons(s) initially generated.
     --> Outputs ntuples.root **(Final file ready for analysis)**
* test/condor/
* test/crab/
