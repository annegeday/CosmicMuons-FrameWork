#!/bin/sh
export X509_USER_PROXY=/afs/cern.ch/user/a/aovergaa/.x509up_96649
voms-proxy-info
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh

cd /eos/user/a/aovergaa/SummerStudent_Project/CMSSW_15_0_5/src/CosmicMuons-FrameWork/Ntuplizer/condor
cmsenv

nMuon=$1
AOD_file=/eos/cms/store/cmst3/group/cosmics/CosmicMuons/SimulationOutput/AODSIM_nMuons${nMuon}.root
ntup_file=/eos/cms/store/cmst3/group/cosmics/CosmicMuons/SimulationOutput/ntuples_nMuons${nMuon}.root
ntupMatch_file=/eos/cms/store/cmst3/group/cosmics/CosmicMuons/SimulationOutput/ntuplesMatched_nMuon${nMuon}.root


cmsRun Cosmics_runNtuplizer_AOD_cfg.py input=${AOD_file} output=${ntup_file}

cd /eos/user/a/aovergaa/SummerStudent_Project/CMSSW_15_0_5/src/CosmicMuons-FrameWork/Simulation

python3 RecoGen_Matching.py ${ntup_file} ${ntupMatch_file}
