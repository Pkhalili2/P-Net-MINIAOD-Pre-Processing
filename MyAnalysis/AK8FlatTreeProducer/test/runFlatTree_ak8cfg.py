import FWCore.ParameterSet.Config as cms
import os 

process = cms.Process("FLATTREE")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1) # Use -1 to run over all events
)
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

input_dirs = [
    # Example: "/hdfs/store/mc/RunIISummer20UL18MiniAODv2/WJetsToLNu_.../",
]

# Automatically create the list of input files
file_list = []
for directory in input_dirs:
    for fname in os.listdir(directory):
        if fname.endswith(".root"):
            file_list.append(f"file:{os.path.join(directory, fname)}")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(*file_list),
    duplicateCheckMode = cms.untracked.string('noDuplicateCheck'),
    skipBadFiles = cms.untracked.bool(True) # Good practice to skip corrupted files
)


process.ak8FlatTree = cms.EDAnalyzer("AK8FlatTreeProducer",
    # InputTag for standard AK8 jets in MINIAOD
    recoJets = cms.InputTag("slimmedJetsAK8"),
    # Standard InputTags for leptons
    muons = cms.InputTag("slimmedMuons"),
    electrons = cms.InputTag("slimmedElectrons"),
    # True for signal samples, False for background
    isSignal = cms.bool(False)
)

process.p = cms.Path(process.ak8FlatTree)