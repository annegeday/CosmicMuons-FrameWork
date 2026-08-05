#!/bin/sh
export X509_USER_PROXY=/afs/cern.ch/user/a/aovergaa/.x509up_96649
voms-proxy-info
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh

cd /eos/user/a/aovergaa/SummerStudent_Project/CMSSW_15_0_5/src/CosmicMuons-FrameWork/Ntuplizer/condor
cmsenv

cmsRun Cosmics_runNtuplizer_AOD_cfg.py input=$1 output=$2
