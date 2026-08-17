#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoMuon/TrackingTools/interface/MuonServiceProxy.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetEnumerators.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "RecoMuon/DetLayers/interface/MuonDetLayerGeometry.h"
#include "RecoMuon/TransientTrackingRecHit/interface/MuonTransientTrackingRecHitBuilder.h"
#include "RecoMuon/TransientTrackingRecHit/interface/MuonTransientTrackingRecHit.h"
#include "RecoMuon/Records/interface/MuonRecoGeometryRecord.h"
#include "RecoMuon/GlobalTrackingTools/interface/StateSegmentMatcher.h"
#include "TTree.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h" //Info about the generated particle
#include "DataFormats/DTRecHit/interface/DTRecHitCollection.h" //Raw DT 1D rechits (dt1DRecHits, kept in AOD)
#include "DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h" //Raw CSC 2D rechits (csc2DRecHits, kept in AOD)


//reco::Muon class: https://github.com/cms-sw/cmssw/blob/master/DataFormats/MuonReco/interface/Muon.h
//Muon POG selections & definitions: https://github.com/cms-sw/cmssw/blob/master/DataFormats/MuonReco/src/MuonSelectors.cc

class MuonServiceProxy;

class MuonNtupleProducer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit MuonNtupleProducer(const edm::ParameterSet&);
  ~MuonNtupleProducer() override = default;

private:
  void beginJob() override;
  void endJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  edm::EDGetTokenT<std::vector<reco::Muon>> muonsToken_;
  edm::EDGetTokenT<DTRecSegment4DCollection> dtSegmentsToken_;
  edm::EDGetTokenT<CSCSegmentCollection> cscSegmentsToken_;
  edm::EDGetTokenT<DTRecHitCollection> dtRecHitsToken_; // All particle DT hits
  edm::EDGetTokenT<CSCRecHit2DCollection> cscRecHitsToken_; // All particle CSC hits
  edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> magneticFieldToken_;
  edm::EDGetTokenT<edm::View<reco::GenParticle> > theGenParticleCollection;
  edm::ParameterSet parameters;
  MuonServiceProxy *theService;
  // output definition
  TTree* tree_;
  TFile *file_;
  std::string output_filename;
  
  static const int MAX = 10000;

  // Event-level info
  unsigned int run_, lumi_, event_;
  int magnetOn_;

  // Reco muon variables
  int nMuons_, muonCharge_[MAX];
  float muonPt_[MAX], muonEta_[MAX], muonPhi_[MAX];
  int muonValidMuonHits_[MAX], muonGlbValidHits_[MAX], muonOuterValidHits_[MAX], muonInnerLayersWithMeasurement_[MAX], muonInnernumberOfValidPixelHits_[MAX], muonMatchedStations_[MAX], muonNumberOfShowers_[MAX];
  int muonIsGlobal_[MAX], muonIsTracker_[MAX], muonIsStandalone_[MAX], muonIsPF_[MAX], muonIsGoodMuon_[MAX];
  float muonInnerDxy_[MAX], muonInnerDz_[MAX], muonInnerValidHitFraction_[MAX];
  float muonSegmentCompatibility_[MAX], muonGlbNormalizedChi2_[MAX], muonOuterNormalizedChi2_[MAX], muonChi2LocalPosition_[MAX], muonCaloCompatibility_[MAX], muonHCALEnergy_[MAX];
  int muonSegmentsDT_[MAX], muonSegmentsCSC_[MAX], muonSegmentsRPC_[MAX];
  
  // Segments
  int   nSeg_;
  int Seg_isDT_[MAX], Seg_isCSC_[MAX], Seg_DTstation_[MAX], Seg_CSCstation_[MAX], Seg_ndof_[MAX];
  float Seg_x_[MAX], Seg_y_[MAX], Seg_z_[MAX];
  float Seg_dirx_[MAX], Seg_diry_[MAX], Seg_dirz_[MAX];
  float Seg_chi2_[MAX];

  // Generated particles
  int nGenMuons_;
  float genMuonPt_[MAX], genMuonEta_[MAX], genMuonPhi_[MAX];


  // Pre-segment hits 
  int nRecHit_;
  int RecHit_isDT_[MAX], RecHit_isCSC_[MAX], RecHit_DTstation_[MAX], RecHit_CSCstation_[MAX];
  float RecHit_x_[MAX], RecHit_y_[MAX], RecHit_z_[MAX];
 
};

MuonNtupleProducer::MuonNtupleProducer(const edm::ParameterSet& iConfig) {
  usesResource("TFileService");
  muonsToken_ = consumes<std::vector<reco::Muon>>(iConfig.getParameter<edm::InputTag>("muons"));
  dtSegmentsToken_ = consumes<DTRecSegment4DCollection>(iConfig.getParameter<edm::InputTag>("segmentsDt"));
  cscSegmentsToken_ = consumes<CSCSegmentCollection>(iConfig.getParameter<edm::InputTag>("segmentsCSC"));
  dtRecHitsToken_ = consumes<DTRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitsDt"));
  cscRecHitsToken_ = consumes<CSCRecHit2DCollection>(iConfig.getParameter<edm::InputTag>("recHitsCSC"));
  magneticFieldToken_ = esConsumes<MagneticField, IdealMagneticFieldRecord>();
  
  parameters = iConfig;
  theGenParticleCollection = consumes<edm::View<reco::GenParticle> > (parameters.getParameter<edm::InputTag>("genParticleCollection"));
  
  edm::ParameterSet serviceParameters = iConfig.getParameter<edm::ParameterSet>("ServiceParameters");
  theService = new MuonServiceProxy(serviceParameters,consumesCollector(),MuonServiceProxy::UseEventSetupIn::Event );
}

void MuonNtupleProducer::beginJob() {

  // // Output file definition
  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>("Events", "Flat muon ntuple");

  // Event info branches 
  tree_->Branch("run", &run_, "run/i");
  tree_->Branch("lumi", &lumi_, "lumi/i");
  tree_->Branch("event", &event_, "event/i");
  tree_->Branch("magnetOn", &magnetOn_, "magnetOn/I");


  // Muon branches
  tree_->Branch("nMuons", &nMuons_, "nMuons/I");
  tree_->Branch("muonPt", muonPt_, "muonPt[nMuons]/F");
  tree_->Branch("muonEta", muonEta_, "muonEta[nMuons]/F");
  tree_->Branch("muonPhi", muonPhi_, "muonPhi[nMuons]/F");
  tree_->Branch("muonCharge", muonCharge_, "muonCharge[nMuons]/I");
  tree_->Branch("muonGlbValidHits", muonGlbValidHits_, "muonGlbValidHits[nMuons]/I");
  tree_->Branch("muonOuterValidHits", muonOuterValidHits_, "muonOuterValidHits[nMuons]/I");
  tree_->Branch("muonInnerLayersWithMeasurement", muonInnerLayersWithMeasurement_, "muonInnerLayersWithMeasurement[nMuons]/I");
  tree_->Branch("muonInnernumberOfValidPixelHits", muonInnernumberOfValidPixelHits_, "muonInnernumberOfValidPixelHits[nMuons]/I");
  tree_->Branch("muonMatchedStations", muonMatchedStations_, "muonMatchedStations[nMuons]/I");
  tree_->Branch("muonNumberOfShowers", muonNumberOfShowers_, "muonNumberOfShowers[nMuons]/I");
  tree_->Branch("muonIsGlobal", muonIsGlobal_, "muonIsGlobal[nMuons]/I");
  tree_->Branch("muonIsTracker", muonIsTracker_, "muonIsTracker[nMuons]/I");
  tree_->Branch("muonIsStandalone", muonIsStandalone_, "muonIsStandalone[nMuons]/I");
  tree_->Branch("muonIsPF", muonIsPF_, "muonIsPF[nMuons]/I");
  tree_->Branch("muonIsGoodMuon", muonIsGoodMuon_, "muonIsGoodMuon[nMuons]/I");
  tree_->Branch("muonInnerDz", muonInnerDz_, "muonInnerDz[nMuons]/F");
  tree_->Branch("muonSegmentCompatibility", muonSegmentCompatibility_, "muonSegmentCompatibility[nMuons]/F");
  tree_->Branch("muonInnerValidHitFraction", muonInnerValidHitFraction_, "muonInnerValidHitFraction[nMuons]/F");
  tree_->Branch("muonGlbNormalizedChi2", muonGlbNormalizedChi2_, "muonGlbNormalizedChi2[nMuons]/F");
  tree_->Branch("muonOuterNormalizedChi2", muonOuterNormalizedChi2_, "muonOuterNormalizedChi2[nMuons]/F");
  tree_->Branch("muonChi2LocalPosition", muonChi2LocalPosition_, "muonChi2LocalPosition[nMuons]/F");
  tree_->Branch("muonCaloCompatibility", muonCaloCompatibility_, "muonCaloCompatibility[nMuons]/F");
  tree_->Branch("muonHCALEnergy", muonHCALEnergy_, "muonHCALEnergy[nMuons]/F");
  tree_->Branch("muonSegmentsDT", muonSegmentsDT_, "muonSegmentsDT[nMuons]/I");
  tree_->Branch("muonSegmentsCSC", muonSegmentsCSC_, "muonSegmentsCSC[nMuons]/I");
  tree_->Branch("muonSegmentsRPC", muonSegmentsRPC_, "muonSegmentsRPC[nMuons]/I");

  // Segment branches
  tree_->Branch("nSeg", &nSeg_, "nSeg/I");
  tree_->Branch("Seg_isDT", Seg_isDT_, "Seg_isDT[nSeg]/I");
  tree_->Branch("Seg_DTstation", Seg_DTstation_, "Seg_DTstation[nSeg]/I");
  tree_->Branch("Seg_isCSC", Seg_isCSC_, "Seg_isCSC[nSeg]/I");
  tree_->Branch("Seg_CSCstation", Seg_CSCstation_, "Seg_CSCstation[nSeg]/I");
  tree_->Branch("Seg_ndof", Seg_ndof_, "Seg_ndof[nSeg]/I");
  tree_->Branch("Seg_x", Seg_x_, "Seg_x[nSeg]/F");
  tree_->Branch("Seg_y", Seg_y_, "Seg_y[nSeg]/F");
  tree_->Branch("Seg_z", Seg_z_, "Seg_z[nSeg]/F");
  tree_->Branch("Seg_dirx", Seg_dirx_, "Seg_dirx[nSeg]/F");
  tree_->Branch("Seg_diry", Seg_diry_, "Seg_diry[nSeg]/F");
  tree_->Branch("Seg_dirz", Seg_dirz_, "Seg_dirz[nSeg]/F");
  tree_->Branch("Seg_chi2", Seg_chi2_, "Seg_chi2[nSeg]/F");

  // GenMuon branches
  tree_->Branch("nGenMuons", &nGenMuons_, "nGenMuons/I");
  tree_->Branch("genMuonPt", genMuonPt_, "genMuonPt[nGenMuons]/F");
  tree_->Branch("genMuonEta", genMuonEta_, "genMuonEta[nGenMuons]/F");
  tree_->Branch("genMuonPhi", genMuonPhi_, "genMuonPhi[nGenMuons]/F");

  // Raw RecHit branches
  tree_->Branch("nRecHit", &nRecHit_, "nRecHit/I");
  tree_->Branch("RecHit_isDT", RecHit_isDT_, "RecHit_isDT[nRecHit]/I");
  tree_->Branch("RecHit_DTstation", RecHit_DTstation_, "RecHit_DTstation[nRecHit]/I");
  tree_->Branch("RecHit_isCSC", RecHit_isCSC_, "RecHit_isCSC[nRecHit]/I");
  tree_->Branch("RecHit_CSCstation", RecHit_CSCstation_, "RecHit_CSCstation[nRecHit]/I");
  tree_->Branch("RecHit_x", RecHit_x_, "RecHit_x[nRecHit]/F");
  tree_->Branch("RecHit_y", RecHit_y_, "RecHit_y[nRecHit]/F");
  tree_->Branch("RecHit_z", RecHit_z_, "RecHit_z[nRecHit]/F");

}
void MuonNtupleProducer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  // Get the event information
  run_ = iEvent.id().run();
  lumi_ = iEvent.luminosityBlock();
  event_ = iEvent.id().event();
  theService->update(iSetup);
  magnetOn_ = 0; //by default assume magnet off

  try {
    const MagneticField& magneticField = iSetup.getData(magneticFieldToken_);
    float bFieldZ = magneticField.inTesla(GlobalPoint(0., 0., 0.)).z();
    magnetOn_ = (std::abs(bFieldZ) > 0.1) ? 1 : 0;
  }catch (cms::Exception& e) {
    edm::LogWarning("MuonNtupleProducer") << "IdealMagneticFieldRecord not available, assuming magnetOff.";
  }
  
  nMuons_ = 0;
  nSeg_ = 0;
  nGenMuons_ = 0;
  nRecHit_ = 0;

  edm::Handle<std::vector<reco::Muon>> muons;
  iEvent.getByToken(muonsToken_, muons);
  
  edm::Handle<DTRecSegment4DCollection> dtSegments;
  iEvent.getByToken(dtSegmentsToken_, dtSegments);
  edm::Handle<CSCSegmentCollection> cscSegments;
  iEvent.getByToken(cscSegmentsToken_, cscSegments);
 
  edm::Handle<DTRecHitCollection> dtRecHits;
  iEvent.getByToken(dtRecHitsToken_, dtRecHits);
  edm::Handle<CSCRecHit2DCollection> cscRecHits;
  iEvent.getByToken(cscRecHitsToken_, cscRecHits);

  edm::Handle<edm::View<reco::GenParticle> > genparticles;
  iEvent.getByToken(theGenParticleCollection, genparticles);

  // Get muon information
  for (const auto& mu : *muons) {
    if (nMuons_ >= MAX) break;
    muonPt_[nMuons_] = mu.pt();
    muonEta_[nMuons_] = mu.eta();
    muonPhi_[nMuons_] = mu.phi();
    muonCharge_[nMuons_] = mu.charge();
    muonGlbValidHits_[nMuons_] = mu.globalTrack().isNonnull() ? mu.globalTrack()->hitPattern().numberOfValidMuonHits() : -1;
    muonOuterValidHits_[nMuons_] = mu.outerTrack().isNonnull() ? mu.outerTrack()->numberOfValidHits() : -1;
    muonInnerLayersWithMeasurement_[nMuons_] = mu.innerTrack().isNonnull() ? mu.innerTrack()->hitPattern().trackerLayersWithMeasurement() : -1;
    muonInnernumberOfValidPixelHits_[nMuons_] = mu.innerTrack().isNonnull() ? mu.innerTrack()->hitPattern().numberOfValidPixelHits() : -1;
    muonMatchedStations_[nMuons_] = mu.numberOfMatchedStations();
    muonNumberOfShowers_[nMuons_] = mu.numberOfShowers();
    muonIsGlobal_[nMuons_] = mu.isGlobalMuon();
    muonIsTracker_[nMuons_] = mu.isTrackerMuon();
    muonIsStandalone_[nMuons_] = mu.isStandAloneMuon();
    muonIsPF_[nMuons_] = mu.isPFMuon();
    muonIsGoodMuon_[nMuons_] = muon::isGoodMuon(mu, muon::TMOneStationTight) ? 1 : 0;

    muonInnerDxy_[nMuons_] = mu.innerTrack().isNonnull() ? mu.innerTrack()->dxy() : -999.0;
    muonInnerDz_[nMuons_] = mu.innerTrack().isNonnull() ? mu.innerTrack()->dz() : -999.0;
    muonSegmentCompatibility_[nMuons_] = muon::segmentCompatibility(mu);
    muonInnerValidHitFraction_[nMuons_] = mu.innerTrack().isNonnull() ? mu.innerTrack()->validFraction() : -999.0;
    muonGlbNormalizedChi2_[nMuons_] = mu.globalTrack().isNonnull() ? mu.globalTrack()->normalizedChi2(): -999.9;
    muonOuterNormalizedChi2_[nMuons_] = mu.outerTrack().isNonnull() ? mu.outerTrack()->normalizedChi2(): -999.9;
    muonChi2LocalPosition_[nMuons_] = mu.combinedQuality().chi2LocalPosition; // always returns 0. To be checked.
    muonCaloCompatibility_[nMuons_] = mu.caloCompatibility();
    muonHCALEnergy_[nMuons_] = mu.calEnergy().had;
      
    muonSegmentsDT_[nMuons_] = 0;
    muonSegmentsCSC_[nMuons_] = 0;
    muonSegmentsRPC_[nMuons_] = 0;
    for (const auto& match : mu.matches()) {
      if (match.detector() == MuonSubdetId::DT) ++muonSegmentsDT_[nMuons_];
      else if (match.detector() == MuonSubdetId::CSC) ++muonSegmentsCSC_[nMuons_];
      else if (match.detector() == MuonSubdetId::RPC) ++muonSegmentsRPC_[nMuons_];
    }
    ++nMuons_;
  }

  // Loop over gen muons, if any 
  if (genparticles.isValid()) { //Only run this loop if the collection is part of the input file (if it is simulated data)
   for (const auto& gp : *genparticles) {
    if (std::abs(gp.pdgId()) != 13) continue; //safeguard to only include muons if we accidentally generate other particles
    if (nGenMuons_ >= MAX) break; // safeguard to avoid overflow of the tree if we accidentally generate too many muons
    genMuonPt_[nGenMuons_] = gp.pt();
    genMuonEta_[nGenMuons_] = gp.eta();
    genMuonPhi_[nGenMuons_] = gp.phi();

    ++nGenMuons_;
    }
  }

  std::map<const GeomDet*, std::vector<TrackingRecHit *> > DetAllSegmentsMap;
  std::map<const GeomDet*, std::vector<TrackingRecHit *> >::iterator it;

  // Loop over DT segments
  for (auto itSeg = dtSegments->begin(); itSeg != dtSegments->end(); itSeg++) {
    //Only valid segments & hasZ & hasPhi
    if(!itSeg->isValid() || !itSeg->hasPhi()) continue;
    DetId myDet = itSeg->geographicalId();
    const GeomDet *geomDet = theService->trackingGeometry()->idToDet(myDet);
    if(geomDet->geographicalId().subdetId()  == MuonSubdetId::DT){
      DTWireId id(geomDet->geographicalId().rawId());
      if(id.station() != 4 && !itSeg->hasZed()){continue;}
      if(id.station() != 4 && itSeg->dimension()!=4){continue;}
    }
    //Get the GeomDet associated to this DetId
    it = DetAllSegmentsMap.find(geomDet);
    if(it == DetAllSegmentsMap.end()) {
      //No -> we create a pair of GeomDet and vector of hits, and put the hit in the vector.
      std::vector<TrackingRecHit *> trhit;
      TrackingRecHit *rechitref = (TrackingRecHit *)&(*itSeg);
      trhit.push_back(rechitref);
      DetAllSegmentsMap.insert(std::pair<const GeomDet*, std::vector<TrackingRecHit *> > (geomDet, trhit));
    } else {
      //Yes -> we just put the hit in the corresponding hit vector.
      TrackingRecHit *rechitref = (TrackingRecHit *) &(*itSeg);
      it->second.push_back(rechitref);
    }
  }

  // Loop over CSC segments
  for (auto itSeg = cscSegments->begin(); itSeg != cscSegments->end(); itSeg++) {
    //Only valid segments     
    if(!itSeg->isValid()) continue;
    DetId myDet = itSeg->geographicalId();
    const GeomDet *geomDet = theService->trackingGeometry()->idToDet(myDet);
    //Get the GeomDet associated to this DetId
    it = DetAllSegmentsMap.find(geomDet);
    if(it == DetAllSegmentsMap.end()) {
      //No -> we create a pair of GeomDet and vector of hits, and put the hit in the vector.
      std::vector<TrackingRecHit *> trhit;
      TrackingRecHit *rechitref = (TrackingRecHit *)&(*itSeg);
      trhit.push_back(rechitref);
      DetAllSegmentsMap.insert(std::pair<const GeomDet*, std::vector<TrackingRecHit *> > (geomDet, trhit));
    } else {
      //Yes -> we just put the hit in the corresponding hit vector.
      TrackingRecHit *rechitref = (TrackingRecHit *) &(*itSeg);
      it->second.push_back(rechitref);
    }
  }


  for (auto it = DetAllSegmentsMap.begin(); it != DetAllSegmentsMap.end(); ++it) {
    if (nSeg_ >= MAX) break;
    const GeomDet* geom = it->first;
    const auto& segVec  = it->second;
    for (size_t i = 0; i < segVec.size(); ++i) {
      const auto* seg = segVec[i];
      LocalPoint  seg_lp  = seg->localPosition();
      GlobalPoint seg_gp  = geom->surface().toGlobal(seg_lp);
      if((*it).second.at(i)->geographicalId().subdetId() == MuonSubdetId::DT){
       DTWireId id((*it).second.at(i)->geographicalId().rawId());
  	Seg_isDT_[nSeg_] =1;
  	Seg_isCSC_[nSeg_] =0;
  	Seg_DTstation_[nSeg_] =id.station();
  	Seg_CSCstation_[nSeg_] =-9999;
  	DTRecSegment4D *mySegment = dynamic_cast<DTRecSegment4D *>((*it).second.at(i));
  	GlobalVector gv = it->first->surface().toGlobal(mySegment->localDirection());
  	Seg_dirx_[nSeg_] =gv.x();
        Seg_diry_[nSeg_] =gv.y();
  	Seg_dirz_[nSeg_] =gv.z();
  	Seg_chi2_[nSeg_] =mySegment->chi2();
        Seg_ndof_[nSeg_] =mySegment->degreesOfFreedom();
      }else if((*it).second.at(i)->geographicalId().subdetId() == MuonSubdetId::CSC){
  	CSCDetId id((*it).second.at(i)->geographicalId().rawId());
  	Seg_isDT_[nSeg_] =0;
  	Seg_isCSC_[nSeg_] =1;
  	Seg_DTstation_[nSeg_] =-9999;
  	Seg_CSCstation_[nSeg_] =id.station();
  	CSCSegment *mySegment = dynamic_cast<CSCSegment *>((*it).second.at(i));
  	GlobalVector gv = it->first->surface().toGlobal(mySegment->localDirection());
  	Seg_dirx_[nSeg_] =gv.x();
  	Seg_diry_[nSeg_] =gv.y();
  	Seg_dirz_[nSeg_] =gv.z();
  	Seg_chi2_[nSeg_] =mySegment->chi2();
  	Seg_ndof_[nSeg_] =mySegment->degreesOfFreedom();
      }
      Seg_x_[nSeg_] =seg_gp.x(); 
      Seg_y_[nSeg_] =seg_gp.y(); 
      Seg_z_[nSeg_] =seg_gp.z();
      ++nSeg_;
     }
  }

// Loop over raw DT 1D rechits (pre-segment-building)
  if (dtRecHits.isValid()) {
    for (auto itHit = dtRecHits->begin(); itHit != dtRecHits->end(); ++itHit) {
      if (nRecHit_ >= MAX) break;
      DetId myDet = itHit->geographicalId();
      const GeomDet *geomDet = theService->trackingGeometry()->idToDet(myDet);
      if (!geomDet) continue;
      GlobalPoint gp = geomDet->surface().toGlobal(itHit->localPosition());
      DTWireId id(myDet.rawId());
      RecHit_isDT_[nRecHit_] = 1;
      RecHit_isCSC_[nRecHit_] = 0;
      RecHit_DTstation_[nRecHit_] = id.station();
      RecHit_CSCstation_[nRecHit_] = -9999;
      RecHit_x_[nRecHit_] = gp.x();
      RecHit_y_[nRecHit_] = gp.y();
      RecHit_z_[nRecHit_] = gp.z();
      ++nRecHit_;
    }
  }
 
  // Loop over raw CSC 2D rechits (pre-segment-building).
  if (cscRecHits.isValid()) {
    for (auto itHit = cscRecHits->begin(); itHit != cscRecHits->end(); ++itHit) {
      if (nRecHit_ >= MAX) break;
      DetId myDet = itHit->geographicalId();
      const GeomDet *geomDet = theService->trackingGeometry()->idToDet(myDet);
      if (!geomDet) continue;
      GlobalPoint gp = geomDet->surface().toGlobal(itHit->localPosition());
      CSCDetId id(myDet.rawId());
      RecHit_isDT_[nRecHit_] = 0;
      RecHit_isCSC_[nRecHit_] = 1;
      RecHit_DTstation_[nRecHit_] = -9999;
      RecHit_CSCstation_[nRecHit_] = id.station();
      RecHit_x_[nRecHit_] = gp.x();
      RecHit_y_[nRecHit_] = gp.y();
      RecHit_z_[nRecHit_] = gp.z();
      ++nRecHit_;
    }
  }
 


  if (nMuons_ > 0 || nGenMuons_ > 0) {
    // Fill the TTree if there is at least one reco::Muon or simulated muon in the event
    tree_->Fill();
  }
}

void MuonNtupleProducer::endJob() 
{

}
DEFINE_FWK_MODULE(MuonNtupleProducer);
