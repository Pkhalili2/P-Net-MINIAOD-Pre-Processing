// File: AK8FlatTreeProducer_updated.cc

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.hh"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "FWCore/Framework/interface/EDConsumerBase.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include <cmath>
#include <algorithm>
#include <vector>
#include <iostream>

typedef std::vector<pat::Jet> JetCollection;

class AK8FlatTreeProducer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit AK8FlatTreeProducer(const edm::ParameterSet&);
  ~AK8FlatTreeProducer() override;

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  void registerBranches(TTree* tree);

  // Tokens to access data from the event
  edm::EDGetTokenT<JetCollection> recoJetsToken_;
  edm::EDGetTokenT<std::vector<pat::Muon>> muonsToken_;
  edm::EDGetTokenT<std::vector<pat::Electron>> electronsToken_;

  // File and TTree pointers for train, validation, and test datasets
  TFile* outFile_train_;
  TFile* outFile_val_;
  TFile* outFile_test_;
  TTree* tree_train_;
  TTree* tree_val_;
  TTree* tree_test_;

  // Variables to be stored in the TTree branches
  std::vector<double> Part_E_, Part_PX_, Part_PY_, Part_PZ_;
  std::vector<double> Part_E_log_, Part_P_, Part_P_log_, Part_Etarel_, Part_Phirel_;

  double truthPX_, truthPY_, truthPZ_, truthE_;
  double E_tot_, PX_tot_, PY_tot_, PZ_tot_, P_tot_, Eta_tot_, Phi_tot_;
  int nPart_, is_signal_new_, idx_, origIdx_, ttv_;

  // Counter to split events
  int eventCounter_ = 0;
};

AK8FlatTreeProducer::AK8FlatTreeProducer(const edm::ParameterSet& iConfig) {
  recoJetsToken_ = consumes<JetCollection>(iConfig.getParameter<edm::InputTag>("recoJets"));
  muonsToken_ = consumes<std::vector<pat::Muon>>(iConfig.getParameter<edm::InputTag>("muons"));
  electronsToken_ = consumes<std::vector<pat::Electron>>(iConfig.getParameter<edm::InputTag>("electrons"));
  is_signal_new_ = iConfig.getParameter<bool>("isSignal");
}

AK8FlatTreeProducer::~AK8FlatTreeProducer() {}

void AK8FlatTreeProducer::registerBranches(TTree* tree) {
  tree->Branch("nPart", &nPart_);
  tree->Branch("Part_E", &Part_E_);
  tree->Branch("Part_PX", &Part_PX_);
  tree->Branch("Part_PY", &Part_PY_);
  tree->Branch("Part_PZ", &Part_PZ_);
  tree->Branch("Part_E_log", &Part_E_log_);
  tree->Branch("Part_P", &Part_P_);
  tree->Branch("Part_P_log", &Part_P_log_);
  tree->Branch("Part_Etarel", &Part_Etarel_);
  tree->Branch("Part_Phirel", &Part_Phirel_);
  tree->Branch("truthPX", &truthPX_);
  tree->Branch("truthPY", &truthPY_);
  tree->Branch("truthPZ", &truthPZ_);
  tree->Branch("truthE", &truthE_);
  tree->Branch("E_tot", &E_tot_);
  tree->Branch("PX_tot", &PX_tot_);
  tree->Branch("PY_tot", &PY_tot_);
  tree->Branch("PZ_tot", &PZ_tot_);
  tree->Branch("P_tot", &P_tot_);
  tree->Branch("Eta_tot", &Eta_tot_);
  tree->Branch("Phi_tot", &Phi_tot_);
  tree->Branch("is_signal_new", &is_signal_new_);
  tree->Branch("idx", &idx_);
  tree->Branch("origIdx", &origIdx_);
  tree->Branch("ttv", &ttv_);
}

void AK8FlatTreeProducer::beginJob() {
  // Create three separate files for training, validation, and testing
  outFile_train_ = new TFile("ak8_train.root", "RECREATE");
  outFile_val_ = new TFile("ak8_val.root", "RECREATE");
  outFile_test_ = new TFile("ak8_test.root", "RECREATE");

  // Setup the TTree's for data
  outFile_train_->cd();
  tree_train_ = new TTree("Events", "Training events for AK8 jets");
  registerBranches(tree_train_);

  outFile_val_->cd();
  tree_val_ = new TTree("Events", "Validation events for AK8 jets");
  registerBranches(tree_val_);

  outFile_test_->cd();
  tree_test_ = new TTree("Events", "Test events for AK8 jets");
  registerBranches(tree_test_);
}

void AK8FlatTreeProducer::analyze(const edm::Event& iEvent, const edm::EventSetup&) {
  // Clear all vectors for each new event
  Part_E_.clear(); Part_PX_.clear(); Part_PY_.clear(); Part_PZ_.clear();
  Part_E_log_.clear(); Part_P_.clear(); Part_P_log_.clear();
  Part_Etarel_.clear(); Part_Phirel_.clear();

  nPart_ = 0;
  E_tot_ = PX_tot_ = PY_tot_ = PZ_tot_ = P_tot_ = Eta_tot_ = Phi_tot_ = -999.;
  truthPX_ = truthPY_ = truthPZ_ = truthE_ = -999.;
  idx_ = iEvent.id().event();
  origIdx_ = iEvent.id().event();
  ttv_ = 0;

  edm::Handle<JetCollection> recoJets;
  edm::Handle<std::vector<pat::Muon>> muons;
  edm::Handle<std::vector<pat::Electron>> electrons;

  iEvent.getByToken(recoJetsToken_, recoJets);
  iEvent.getByToken(muonsToken_, muons);
  iEvent.getByToken(electronsToken_, electrons);

  if (recoJets.isValid()) {
    const pat::Jet* selectedJet = nullptr;
    float max_pt = -1.0;

    // Loop over all jets in the event to find the leading one that passes selection
    for (const auto& jet : *recoJets) {
      // Apply jet kinematic cuts 
      if (jet.pt() < 170 || std::abs(jet.eta()) > 3.0) continue;

      // Check for a valid, isolated lepton back-to-back with the jet
      bool has_valid_lepton = false;
      if (muons.isValid()) {
          for (const auto& mu : *muons) {
            // Apply stricter muon selection criteria
            if (mu.pt() > 30 && std::abs(mu.eta()) < 2.5 && mu.pfIsolationR04().sumChargedHadronPt / mu.pt() < 0.3) {
              if (reco::deltaPhi(jet.phi(), mu.phi()) > 1.5) {
                has_valid_lepton = true;
                break;
              }
            }
          }
      }
      if (has_valid_lepton) break; // No need to check electrons if a muon is found
      
      if (electrons.isValid()) {
          for (const auto& ele : *electrons) {
            // Apply stricter electron selection criteria
            if (ele.pt() > 30 && std::abs(ele.eta()) < 2.5 && ele.pfIsolationVariables().sumChargedHadronPt / ele.pt() < 0.3) {
              if (reco::deltaPhi(jet.phi(), ele.phi()) > 1.5) {
                has_valid_lepton = true;
                break;
              }
            }
          }
      }

      // Skip this jet if no valid lepton is found
      if (!has_valid_lepton) continue;

      // If the jet passes all selections, check if it's the highest-pT one so far
      if (jet.pt() > max_pt) {
        max_pt = jet.pt();
        selectedJet = &jet;
      }
    }

    // If a jet was selected, process its constituents
    if (selectedJet) {
      E_tot_ = selectedJet->energy();
      PX_tot_ = selectedJet->px();
      PY_tot_ = selectedJet->py();
      PZ_tot_ = selectedJet->pz();
      P_tot_ = selectedJet->p();
      Eta_tot_ = selectedJet->eta();
      Phi_tot_ = selectedJet->phi();

      // Get the matched generator-level jet information (truth)
      const reco::GenJet* genJet = selectedJet->genJet();
      if (genJet) {
        TLorentzVector genVec;
        genVec.SetPtEtaPhiE(genJet->pt(), genJet->eta(), genJet->phi(), genJet->energy());
        truthPX_ = genVec.Px();
        truthPY_ = genVec.Py();
        truthPZ_ = genVec.Pz();
        truthE_ = genVec.E();
      }

      // Loop over the jet's constituent particles
      for (const auto& cand : selectedJet->daughterPtrVector()) {
        const pat::PackedCandidate* pf = dynamic_cast<const pat::PackedCandidate*>(cand.get());
        if (!pf) continue;

        double p = pf->p();
        double e = pf->energy();
        double dEta = pf->eta() - selectedJet->eta();
        double dPhi = reco::deltaPhi(pf->phi(), selectedJet->phi());

        // Store particle properties
        Part_E_.push_back(e);
        Part_PX_.push_back(pf->px());
        Part_PY_.push_back(pf->py());
        Part_PZ_.push_back(pf->pz());
        Part_E_log_.push_back(log(e + 1e-6));
        Part_P_.push_back(p);
        Part_P_log_.push_back(log(p + 1e-6));
        Part_Etarel_.push_back(dEta);
        Part_Phirel_.push_back(dPhi);
      }

      nPart_ = Part_E_.size();
      
      // Only process events that have at least one constituent
      if (nPart_ > 0) {
        // Increment the event counter for splitting
        ++eventCounter_;

        // Split the events into test (10%), validation (10%), and train (80%)
        if (eventCounter_ % 10 == 0) {
            ttv_ = 2; // Label as test
            tree_test_->Fill();
        } else if (eventCounter_ % 10 == 9) {
            ttv_ = 1; // Label as validation
            tree_val_->Fill();
        } else {
            ttv_ = 0; // Label as train
            tree_train_->Fill();
        }
      }
    }
  }
}

void AK8FlatTreeProducer::endJob() {
  // Write and close all three output files
  outFile_train_->cd();
  tree_train_->Write();
  outFile_train_->Close();

  outFile_val_->cd();
  tree_val_->Write();
  outFile_val_->Close();

  outFile_test_->cd();
  tree_test_->Write();
  outFile_test_->Close();
}

DEFINE_FWK_MODULE(AK8FlatTreeProducer);