# Cosmics Muons Ntupler + Simulation Framework
September 9th: README currently being updated and restructured 

## Instructions for installing
*The patch has yet to be verified. Full installation (cloning+patches) should be tested from a new working area.*

### Installing CMSSW

    cmsrel CMSSW_15_0_5
    cd CMSSW_15_0_5/src
    cmsenv

### Setting up the framework
From CMSSW_15_0_5/src, run: 

    git clone git@github.com:fmanteca/CosmicMuons-FrameWork.git
    scram b -j 8

To run the framework, we need to include the corresponding additions/modifications of certain CMSSW packages, which are saved as a patch in this repository. From CMSSW_15_0_5/src, run:

    git cms-addpkg IOMC/ParticleGuns
    git cms-addpkg IOMC/EventVertexGenerators
    git cms-addpkg Configuration/Generator
    git cms-addpkg RecoLocalMuon/Configuration

    git apply CosmicMuons-FrameWork/CosmicMuons.patch
    scram b -j 8

## Overview : Directory structure and file descriptions

### DataSegment_AOD/
Contains files related to generation of AOD files (that contain all segment and hit info) from cosmic RAW data files.

* CosmicPPreco_RAW2DIGI_RECO.py
  + Takes raw cosmic data files and generates AOD files including segment data.
  + Configuration options
    - FileString: Path to raw data file in string format (Don't forget file: for a local file).
    - MaxEvts: Number of events from the selected file(s), that you'd like to process. Standard is -1, corresponding to all events.
    - numCore: Number of cores allocated for job. Standard is 1 (2 for a crab job).
  + Output: AOD file with name "CosmicPPreco_RAW2DIGI_RECO.root" in working directory. 
* crab_CosmicPPreco.py
  + Submits job to crab in order to process many data files.
  + Configuration options 
    - config.JobType.numCores: **Must** match numCores in CosmicPPreco_RAW2DIGI_RECO.py.
    - config.JobType.maxMemoryMB: Memory allocated, depends on number of cores (1 core: 2500-3000)
    - config.Data.outLFNDirBase: Placement of output directory
    - config.Data.outputDatasetTag: Name of output directory
    - config.Data.inputDataset: DAS dataset name (for txt list of datafiles, use userInputFiles and outputPrimaryDataset options instead)
  + Output: AOD files with names "CosmicPPreco_RAW2DIGI_RECO_{JOB-NUMBER}.root" in generated output directory.

### Simulation/
Contains files related to generation and simulation of cosmic muon showers through CMS.

* MultiCosmicGun_GEN_SIM_cfg.py
  + Simulates cosmic muon showers by generating them above the detector with momentum going towards the detector. Each muon is generated with a random origin along the  axis parallel to the beamline. The direction of the shower is determined by Max/Min eta.
  + Input params
    - nMuons: Number of muons generated per event/muons in the shower. No default, **Must** be specified when calling the script, ex: `cmsRun MultiCosmicGun_GEN_SIM_cfg.py nMuons=5`
    - nEvents: Number of events generated. Default is 1000.
    - output: Path for output file. Default is 'GEN-SIM_MultiCosmic.root'.
  + Configuration options (Shower settings):
    - MaxZenithAngle: The maximum allowed zenith angle of shower core. Standard is 1.047 corresponding to ~60 degrees.
    - ShowerHalfWidth: The allowed deviation of muon direction from shower core.
  + Outputs root file in accordance with output path.
* GEN_SIM_to_AOD_cfg.py
  + Takes file containing simulated muons and generates "data-like" AOD files.
  + Input params
    - input: Path for input file. Default is 'GEN-SIM_MultiCosmic.root'.
    - output: Path for output file. Default is 'AODSIM.root'.
  + Outputs AOD root file in accordance with output path.
* RecoGen_Matching.py
  + Takes Ntuplizer output file for simulated muons and matches generated and reconstructed muons via their Lorentz vector. Simulated muons are 1-to-1 paired with their closest match, provided their Eucledian distance is below a predetermined threshold.
  + Configuration options
    - dR_max: Threshold for a match
  + Input params
    - Input file path. Default is ntuples.root in working directory.
    - Output file path. Default is ntuplesMatched.root in working directory.
    - To specify input and output path, add these when calling the script: `python3 RecoGen_Matching.py {InputFile} {OutputFile}`
  + Outputs a root file containing a TTree in accordance with output path. 
* condor/
  + prepare_sim_input.py
    - Creates text file with nMuons,nEvents.
    - Configuration options
          nMuons_list: List with nMuons for each desired job.
          output_txt_file: txt output path.
          nEvents_constant: Set true if each job should have the same number of events.
          nEvents: Number of events per job (if nEvents_constant = True).
          nEvents_list: List with number of events for each job. Job index should match with nMuons_list (if nEvents_constant = False).
  + run.sh
    - Shell script to cd to working directory, ensure safe file names and call both simulation scripts, one after the other.
    - Configuration options: set your own working directory, last two lines can be commented in or out, depending on whether you want to keep the intermediate GEN-SIM files after simulation. 
  + condor.sub
    - Submits the condor job following the shell script when command ´condor_submit condor.sub´ is called.
    - Configuration options: path to .txt file with nMuons,nEvents.

### Ntuplizer/
Contains files related to generation of Ntuples from AOD files. 

* Cosmics_runNtuplizer_AOD_cfg.py
  + Takes the MuonNtupleProducer plugin and produces Ntuples including segment and hit info. If the data comes from simulated muons, the output will contain info regarding the muons(s) that were generated for each event.
  + Only accepts data that contains all neccessary segment+hit info. Therefore does not accept cosmic files in RECO format directly from DAS.
  + Configuration options
    - inputPath: Path to AOD data file in string format (Don't forget file: for a local file)
  + Outputs ntuples.root in working directory
  + Run with `cmsRun Cosmics_runNtuplizer_AOD_cfg.py`
* condor/
  + Cosmics_runNtuplizer_AOD_cfg.py
    - Version of the Ntuplizer for condor (feeds from plugin/MuonNtupleProducer just like the non-condor Ntuplizer)
  + data_input/
    - prepare_files.py
      * Takes dataset path(s) and generates a .txt file with all the corresponding file names, 1 per line.
      * Input: dataset path(s)
      * Output: 'files.txt' in output base
    - run.sh
      * Shell script to access grid proxy, find condor working directory and start job
      * Configuration options: X509_USER_PROXY and working directory
    - condor.sub
      * Submits the condor job following the shell script when called ´condor_submit condor.sub´
      * Configuration options: .txt file path
  + sim_input/
    - run.sh
      * Shell script to access grid proxy, find condor working directory and start Ntuplizer, then run matching script when finished.
      * Paths to working directories 
    - condor.sub
      * Submits the condor job following the shell script when called ´condor_submit condor.sub´
      * Configuration options: .txt file path
    - nMuons_lineSep.txt
      * Simply a txt document with number of muons that should be generated per run. One integer per line.

### Patch_References/
Contains read-only copies of the files modified/added by `CosmicMuons.patch`. These are just included for convenience so the changes can be browsed easily without applying the patch, but the framework has no direct dependence on these copies. 

* RecoLocalMuon/Configuration/python/RecoLocalMuonCosmics_EventContent_cff.py
* Configuration/Generator/python/MultiCosmicGun_cfi.py
* IOMC/
  + EventVertexGenerators/
    - interface/MultiVtxFlatEvtVtxGenerator.h
    - src/MultiVtxFlatEvtVtxGenerator.cc
    - /src/module.cc
  + ParticleGuns/
    - interface/MultiVtxFlatRandomPtGunProducer.h
    - src/MultiVtxFlatRandomPtGunProducer.cc
    - src/SealModule.cc
   
## Tips&tricks

### Submit jobs to HTCondor
There are scripts to submit the different jobs to HTCondor. For each run, condor.sub will submit one job per row of an appropriate txt file to the cluster, taking run.sh as the executable.
Condor jobs can not be submitted from an eos/ path, therefore you must copy the files you need (run.sh, condor.sub & .txt-file) to your afs/ directory, and submit from there. You must also edit the files to ensure all paths point to your personal working directories. 
If you are accessing data, you must also ensure that X509_USER_PROXY points to a valid grid proxy. 
Finally submit the job with:

    condor_submit condor.sub

To check on the status of your jobs, type: 

    condor_q
    
or, for more detail, find the relevant job ID and run:

    condor_q -better-analyze <job_id>

#### Example: Submit Ntuplizer job for a full dataset
Before running the following commands, set your personal path in run.sh and select the input datasets and output path in prepare_files.py.
prepare_files.py will produce a txt file where each row will contain a pair input_file output_file. 

    cd condor
    voms-proxy-init --voms cms --hours 96  -out ${HOME}/.x509up_${UID};export X509_USER_PROXY=${HOME}/.x509up_${UID}
    python3 prepare_files.py
    condor_submit condor.sub

Merge the outputs with hadd. Example:

    hadd /eos/cms/store/group/phys_muon/fernanpe/Cosmics2025/merged.root /eos/cms/store/group/phys_muon/fernanpe/Cosmics2025/*/*root
    
### Submit jobs to crab
The following instructions will allow to submit large jobs to the cluster by making a local copy of your CMSSW area. For example, it can take a full dataset. 
This option is also useful when the dataset is no longer available on disk, as crab will request a copy on disk automatically before starting running the jobs.

The output can be stored in your '/eos/user/' area if T3_CH_CERNBOX is set as storage site.


#### Example: Generate AOD files for a full raw data set.
Edit crab_CosmicPPreco.py to point to the desired input data set and correct output path. 
Ensure that a sufficient amount of cores and memory is allocated for the job, as this is difficult to solve later (For the Commissiong2025 set, 3 cores were needed).
Finally, submit the job:

    source /cvmfs/cms.cern.ch/crab3/crab.sh
    cmsenv
    voms-proxy-init --voms cms --valid 168:00
    crab submit crab_CosmicPPreco.py


### Event displays with Fireworks
Use Fireworks to generate event displays (accepts miniAOD/AOD/RECO formats as input).

You can use edmPickEvents.py to filter out events from a full data set, if needed (see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookPickEvents).

Copy the output to a public /eos/cms/store/group/ path. There is also the possibility to read files from your /eos/user/ private path. Follow the instructions here: https://github.com/alja/FireworksWeb/blob/main/doc/UserGuide.md

Insert path here: [https://fireworks.cern.ch/cmsShowWeb/revetor.pl](https://fireworks.cern.ch/cmsShowWeb/service.pl) (starting with /store/)

Instructions for cosmic multi-muons:
* Add Collections -> type "leg" -> select "Muons muon1Leg"
* FilterDialog -> $Muons10_muons1Leg@.size()>10 
* 3D -> i -> Geometry -> show all

### Useful links:
* Muon reconstruction documentation: https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuons
* Cosmic muon reconstruction documentation: https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideCosmicMuonReco
* reco::Muon class: https://github.com/cms-sw/cmssw/blob/master/DataFormats/MuonReco/interface/Muon.h
* Muon POG selections & definitions: https://github.com/cms-sw/cmssw/blob/master/DataFormats/MuonReco/src/MuonSelectors.cc
* OMS (used to get run numbers from CRUZET/CRAFT): https://cmsoms.cern.ch/cms/run_3_cruz/cruzet_2025?cms_run_sequence=GLOBAL-RUN
* Get the datasets containing a given run number in DAS: https://cmsweb.cern.ch/das/request?view=list&limit=50&instance=prod%2Fglobal&input=dataset+run%3D389767


## Considerations during framework developement process 

### Including segment and hit info in data files
It turns out that, by default, CMS saves muon segments in AOD for pp collision runs, but not for cosmics. One has to add them [here:](https://github.com/cms-sw/cmssw/blob/master/RecoLocalMuon/Configuration/python/RecoLocalMuonCosmics_EventContent_cff.py#L5). Benchmark from pp cfg: [here](https://github.com/cms-sw/cmssw/blob/master/RecoLocalMuon/Configuration/python/RecoLocalMuon_EventContent_cff.py#L7-L13).

The script to produce customized AOD (with muon segments + DT/CSC hist info kept) from RAW data, is located in the DataSegment_AOD/ directory. See [here](#DataSegment_AOD)

Crab is the best solution for running this type of task. Submit jobs to crab taking /Cosmics/Commissioning2025-v1/RAW as the input dataset:

     crab submit  crab_CosmicPPreco.py

The first version of the CosmicPPreco_RAW2DIgi_RECO.py script was generated in the following way: 

    git cms-addpkg RecoLocalMuon/Configuration
    <Edit RecoLocalMuon/Configuration/python/RecoLocalMuonCosmics_EventContent_cff.py>
    scram b -j 20
    cmsDriver.py CosmicPPreco --step RAW2DIGI,RECO --datatier AOD --eventcontent AOD --filein=/store/data/Run2024C/Cosmics/RAW/v1/000/379/417/00000/022b1b63-e126-4800-be9a-cbd752664a95.root --fileout file:CosmicPPreco_RAW2DIGI_RECO.root --conditions 140X_dataRun3_Prompt_v2 --era Run3 --scenario cosmics --data -n 100


### Producing Ntuples
The framework will produce Ntuples from AOD. Every time you make a change anywhere, for instance in 'plugins/MuonNtupleProducer.cc', do not forget to re-compile (run 'scram b -j 8' in 'CMSSW_15_0_5/src').
The current version of the Ntuplizer will only accept input files that contain info about segments and hits. This could be accounted for in the same way as has been done for GenParticles. If the input AOD files are simulated, the Ntuplizer will keep info about the generated particles. 
Instructions can be found in the [section describing the Ntuplizer directory](#Ntuplizer) .

### Simulation studies

We will make use of the `FlatRandomPtGunProducer` to generate guns of cosmic muons. It is designed to simulate collisions, meaning it generates particles exactly at the center of the detector (0,0,0) and fires them outwards. If one uses the module as is, the muons will be born at the center. Half will go upwards (passing only through the top half) and half downwards (passing only through the bottom half). To get them to completely pass through the detector from top to bottom, simulating cosmic rays, we have tricked CMSSW by doing a couple of things: 

1. Restricting the angles of each event so that the "Gun" only fires downwards.
2. Assign an overall shower angle to each event by drawing from an appropriate distribution 
3. Generate one vertex per muon
4. Move each collision vertex individually to the top of the detector cavern, and a assign a specific angle from a small range around the shower.

The above 4 modifications are described in some (but not all) detail below.
To include these changes, modifications and addition were made to the CMSSW pre-defined packages. These are all included in the CosmicMuons patch. 

#### 1. The Generator: Firing Downwards.

In the CMS coordinate system, the Y-axis points upwards. The azimuthal angle $\phi$ dictates the direction in the transverse plane: $\phi = +\pi/2$ is upwards. $\phi = -\pi/2$ is directly downwards. 
Below is the block of code where we define parameteres for the particle gun. We configure the generator to fire muons that go exclusively in the lower hemisphere (downwards) by restricting Max/Min Phi to a narrow window near $\phi = -\pi/2$. The number of muons is set by multiplying the particleID for a muon (\[13\]) with the variable integer of generated muons. This is also where particle momemtum can be adjusted. Default is a random value between 100 and 3000 GeV.

In MultiCosmicGun_GEN_SIM_cfg.py: 

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

#### 2. The Particle Gun: Assign an angle as the shower core for each event
Because the particle gun is intented to simulate collisions, it does not have the option to make event-specific angular restrictions, so we have to find a way to include this ourselves. 
For each produced event, we want all muons to have very similar arrival angles, but we want these arrival angle to vary for each event. To solve this, every time the Particle Gun module starts a new event, it draws an angle from a symmetric $\cos^2\left(\alpha\right)$ distribution, where $\alpha$ is the zenith angle/arrival of the cosmic ray. $\alpha$ is capped by the variable MaxZenithAngle in MultiCosmicGun_GEN_SIM_cfg.py. This choice of distribution is standard from cosmic ray litterature. See where this adjustment to the particle gun is implemented below.

In MultiVtxFlatRandomPtGunProducer.cc:

    \\ (...) Beginning of event loop above, then 
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
  
      double evtMinEta = etaMid - fShowerHalfWidth;
      double evtMaxEta = etaMid + fShowerHalfWidth;

This shows that a the beginning of an event loop, we have determined evtMinEta and evtMaxEta for particles contained in that event. 

#### 3. The Vertex Producer: Generate a vertex for each individual muon
The standard vertex generator initially used for the generator, only assigns a single vertex per event, thus generating every muon at the same xyz-coordinate (usually set for collisions to (0,0)). As muons with the same cosmic shower are seperated in space, we want an individual vertex per muon. This requires that our new vertex generator assigns a random X-Y- and Z- value for each muon, which happens in the loop described below. 

In MultiVtxFlatEvtVtxGenerator.cc : 
  
      HepMC::GenEvent* genevt = new HepMC::GenEvent(*HepUnsmearedMCEvt->GetEvent());

      for (auto vtxIt = genevt->vertices_begin(); vtxIt != genevt->vertices_end(); ++vtxIt) {
        double aX = CLHEP::RandFlat::shoot(engine, fMinX, fMaxX);
        double aY = CLHEP::RandFlat::shoot(engine, fMinY, fMaxY);
        double aZ = CLHEP::RandFlat::shoot(engine, fMinZ, fMaxZ);
        double aT = CLHEP::RandFlat::shoot(engine, fMinT, fMaxT);
        (*vtxIt)->set_position(HepMC::FourVector(aX, aY, aZ, aT));
    }

      std::unique_ptr<edm::HepMCProduct> HepMCEvt(new edm::HepMCProduct(genevt));
      evt.put(std::move(HepMCEvt));

#### 4. Vertex Smearing: Move each origin to the top.

Now we need to tell CMSSW that these particles are not born at the center $(0,0,0)$, but on an imaginary plane above the detector (for example, at $Y = +800$ cm, just outside the muon barrel). To do this, we use the process called "Vertex-smearing". This is intended to allow offsets in $(0,0,0)$, but if we force MinY = MaxY = 800, we fix the vertex to a plane right above the detector. The size of the plane is determined by Min/Max for X and Y, and is set to 10x12 meters 

```
# =========================================================
# HACK: Move the origin of the muons to the top of the cavern
# =========================================================
process.VtxSmeared = cms.EDProducer("MultiVtxFlatEvtVtxGenerator",
    MinX = cms.double(-500.0), # Transversal area: 10 meters
    MaxX = cms.double(500.0),
    MinY = cms.double(800.0),  # Origin at 8 meter height (on top of the detector)
    MaxY = cms.double(800.0),
    MinZ = cms.double(-600.0), # Longitudinal length: 12 meters
    MaxZ = cms.double(600.0),
    MinT = cms.double(0.0),
    MaxT = cms.double(0.0),
    # TimeOffset = cms.double(0.0), # TimeOffset is not read in MultiVtxFlatEvtGenerator, only original VtxFlatEvtGenerator
    src = cms.InputTag("generator", "unsmeared")
)
```

Finally, after all these implementations, we can run the generator, ex:

     cmsRun MultiCosmicGun_GEN_SIM_cfg.py nMuons=4

The output file should contain GEN-SIM information. You can check out the event content by running:

     edmDumpEventContent GEN-SIM_MultiCosmic.root

#### 3. Produce AOD

The last step consists of running the GEN-SIM -> AOD step, so that we have the same data format as in data, but including information from the generated particles.
Unfortunately, there is not a dedicated `--condition` to do this step for Run3 cosmics MC so far, so the file GEN_SIM_to_AOD_cfg.py is based on the conditions from data, and modified to ensure that the generated information is stored in the AOD output.
By doing this, the generated particle kinematics, generator information, and the simulated muon hits will be stored in the AOD output.

Running a GEN-SIM file through GEN_SIM_to_AOD_cfg.py thus makes simulated data look like real data, but containing the "truth" about injected/generated muons. 
Meaning one can now feed FireWorks with the AOD output file, make the flat ntuples, etc. 
