import FWCore.ParameterSet.Config as cms
import os

process = cms.Process("FLATTREE")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

input_dirs = [
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT400_600",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT600_800",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT100_200",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT200_400",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT2500_Inf",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT1200_2500",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/8bjets"
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/8bjets_Extra"
]

file_list = []
for directory in input_dirs:
    for fname in os.listdir(directory):
        if fname.endswith(".root"):
            file_list.append(f"file:{os.path.join(directory, fname)}")

# specific_files = [
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_91_Mini.root",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_92_Mini.root",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_93_Mini.root",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_94_Mini.root",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_95_Mini.root",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_96_Mini.root",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_97_Mini.root",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_98_Mini.root",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_99_Mini.root",
    # "/hdfs/store/user/abdollah/ExoHiggs_WithAK15/MiniAOD/WJet_HT800_1200/OUTROOT_WJet_HT800_1200_9_Mini.root",
# ]
# file_list.extend([f"file:{f}" for f in specific_files])

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(*file_list),
    duplicateCheckMode = cms.untracked.string('noDuplicateCheck'),
    skipBadFiles = cms.untracked.bool(True)
)


# process.TFileService = cms.Service("TFileService",
    # fileName = cms.string("ak15_Extract.root")
# )

process.ak15FlatTree = cms.EDAnalyzer("AK15FlatTreeProducer",
    recoJets = cms.InputTag("selectedPatJetsAK15PFCHS", "", "NANO"),
    muons = cms.InputTag("slimmedMuons"),
    electrons = cms.InputTag("slimmedElectrons"),
    isSignal = cms.bool(True)
)

process.p = cms.Path(process.ak15FlatTree)
