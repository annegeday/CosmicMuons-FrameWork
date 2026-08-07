#!/bin/sh
export X509_USER_PROXY=/afs/cern.ch/user/a/aovergaa/.x509up_196026.
voms-proxy-info
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh

cd /eos/user/a/aovergaa/SummerStudent_Project/CMSSW_15_0_5/src/CosmicMuons-FrameWork/Simulation/condor
cmsenv

NMUONS=$1
NEVENTS=$2

# use cluster and process to give output files unique names
CLUSTER=$3
PROCESS=$4

GENSIM_FILE=GEN-SIM_nMuons${NMUONS}_${CLUSTER}_${PROCESS}.root
AOD_FILE=AODSIM_nMuons${NMUONS}_${CLUSTER}_${PROCESS}.root

echo "Step 1/2: GEN,SIM with nMuons=${NMUONS}, nEvents=${NEVENTS}"
cmsRun MultiCosmicGun_GEN_SIM_cfg.py nMuons=${NMUONS} nEvents=${NEVENTS} output=${GENSIM_FILE}

if [ ${STEP1_STATUS} -ne 0 ] || [ ! -f ${GENSIM_FILE} ]; then
    echo ">>> ERROR: Step 1 (GEN,SIM) failed or did not produce ${GENSIM_FILE} -- aborting before step 2"
    exit 1
fi

echo "Step 2/2: GEN-SIM -> AODSIM"
cmsRun GEN_SIM_to_AOD_cfg.py input=${GENSIM_FILE} output=${AOD_FILE}

# echo "Cleaning up intermediate GEN-SIM file"
# rm -f ${GENSIM_FILE}
