import ROOT
import sys

# ---- Check correct input format or go to default ---- #
if len(sys.argv) != 3 and len(sys.argv) != 1:
   print(" USAGE : %s <ntuple input file> <ntuple output file>"%(sys.argv[0]))
   sys.exit(1)

if len(sys.argv) ==  3:
   inFileName = sys.argv[1]
   outFileName = sys.argv[2]

if len(sys.argv) ==  1:
   inFileName = "ntuples.root"
   outFileName = "ntuplesMatched.root"

# ---- File/tree paths and definitions ---- #

inFile = ROOT.TFile.Open(inFileName, "READ")
inTree = inFile.Get("muonNtupleProducer/Events")

outFile = ROOT.TFile.Open(outFileName, "CREATE")
outTree = ROOT.TTree("MatchedEvents", "1-to-1 gen-reco muon matching")

muonMass = 0.106 #GeV
dR_max = 0.5 #Threshold for match

# ---- New tree branches ---- #

matched_genIdx = ROOT.std.vector("int")()
matched_recoIdx = ROOT.std.vector("int")()
matched_dR = ROOT.std.vector("float")()

unmatched_genIdx = ROOT.std.vector("int")()
unmatched_recoIdx = ROOT.std.vector("int")()

nMatched = ROOT.std.vector("int")(1)  # placeholder for initialization

outTree.Branch("matched_genIdx", matched_genIdx)
outTree.Branch("matched_recoIdx", matched_recoIdx)
outTree.Branch("matched_dR", matched_dR)
outTree.Branch("unmatched_genIdx", unmatched_genIdx)
outTree.Branch("unmatched_recoIdx", unmatched_recoIdx)


# ---- Matching loop ---- #

for entryNum in range(0,inTree.GetEntries()):
   inTree.GetEntry(entryNum)

   # ---- Get event branch data ---- #

   nMuons = getattr(inTree, "nMuons") #number of reco muons
   nGenMuons = getattr(inTree, "nGenMuons") #number of gen muons

   pt  = getattr(inTree, "muonPt")
   eta = getattr(inTree, "muonEta")
   phi = getattr(inTree, "muonPhi")

   ptGen  = getattr(inTree, "genMuonPt")
   etaGen = getattr(inTree, "genMuonEta")
   phiGen = getattr(inTree, "genMuonPhi")

   # ---- Build 4-vectors ---- #

   #loop over reconstructed muons
   vecMuons = []
   for i in range(nMuons):
       muon = ROOT.TLorentzVector()
       muon.SetPtEtaPhiM(pt[i], eta[i], phi[i], muonMass)
       vecMuons.append(muon)

   #loop over generated muons
   vecGenMuons = []
   for i in range(nGenMuons):
       genMuon = ROOT.TLorentzVector()
       genMuon.SetPtEtaPhiM(ptGen[i], etaGen[i], phiGen[i], muonMass)
       vecGenMuons.append(genMuon)



   # ---- Find all possible matches ---- #

   candidateMatches = []

   for genIdx, genMuon in enumerate(vecGenMuons):
      for muIdx, muon in enumerate(vecMuons):
         dR = genMuon.DeltaR(muon)
         if dR < dR_max:
            candidateMatches.append((dR, genIdx, muIdx))


   # ---- Create pairs, prioritizing closest matches ---- #

   candidateMatches.sort(key=lambda c: c[0]) #sort by ascending dR, such that loop prioritizes close matches

   pairs = []
   usedGen = set()
   usedReco = set()

   for dR, genIdx, muIdx in candidateMatches:
          if genIdx in usedGen or muIdx in usedReco:
             continue
          pairs.append((genIdx, muIdx, dR))
          usedGen.add(genIdx)
          usedReco.add(muIdx)

   #unmatched muons
   unmatchedGens = [g for g in range(len(vecGenMuons)) if g not in usedGen]
   unmatchedRecos = [r for r in range(len(vecMuons)) if r not in usedReco]


   # ---- Fill branches ---- #

   matched_genIdx.clear()  #clear vector between events
   matched_recoIdx.clear()
   matched_dR.clear()
   unmatched_genIdx.clear()
   unmatched_recoIdx.clear()

   for genIdx, muIdx, dR in pairs:
      matched_genIdx.push_back(genIdx)  #push_back appends to a vector in c++
      matched_recoIdx.push_back(muIdx)
      matched_dR.push_back(dR)
   for g in unmatchedGens:
      unmatched_genIdx.push_back(g)
   for r in unmatchedRecos:
      unmatched_recoIdx.push_back(r)

   outTree.Fill()  #add final vectors to the tree


# ---- After looping over all events: write tree to output, close filesz and print status ---- #

outFile.cd()
outTree.Write()
outFile.Close()
inFile.Close()

print("Done. Wrote Ttree to %s" % (outFileName))
