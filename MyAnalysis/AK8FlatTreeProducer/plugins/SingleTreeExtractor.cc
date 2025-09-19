// File: SingleTreeExtractor.cc

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "TFile.h"
#include "TTree.h"

#include <string>

class SingleTreeExtractor : public edm::one::EDAnalyzer<> {
public:
  explicit SingleTreeExtractor(const edm::ParameterSet&);
  ~SingleTreeExtractor() override = default;

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override {}
  void beginJob() override;
  void endJob() override {}

  std::string inputFilePath_;
  std::string outputFilePath_;
};

SingleTreeExtractor::SingleTreeExtractor(const edm::ParameterSet& iConfig) {
  inputFilePath_ = iConfig.getUntrackedParameter<std::string>("inputFile");
  outputFilePath_ = iConfig.getUntrackedParameter<std::string>("outputFile");
}

void SingleTreeExtractor::beginJob() {
  TFile* inFile = TFile::Open(inputFilePath_.c_str(), "READ");
  if (!inFile || inFile->IsZombie()) {
    edm::LogError("SingleTreeExtractor") << "Failed to open input file: " << inputFilePath_;
    return;
  }

  TTree* tree = dynamic_cast<TTree*>(inFile->Get("Events"));
  if (!tree) {
    edm::LogError("SingleTreeExtractor") << "TTree 'Events' not found in input file.";
    inFile->Close();
    return;
  }

  edm::LogInfo("SingleTreeExtractor") << "Found TTree 'Events' with " << tree->GetEntries() << " entries.";

  TFile* outFile = new TFile(outputFilePath_.c_str(), "RECREATE");
  outFile->cd();
  TTree* newTree = tree->CloneTree(-1); 
  newTree->Write("Events");
  outFile->Close();
  inFile->Close();

  edm::LogInfo("SingleTreeExtractor") << "TTree successfully written to: " << outputFilePath_;
}

DEFINE_FWK_MODULE(SingleTreeExtractor);
