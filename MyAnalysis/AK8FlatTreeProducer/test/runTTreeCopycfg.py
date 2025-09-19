import FWCore.ParameterSet.Config as cms

process = cms.Process("COPYTREE")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 5000

process.source = cms.Source("EmptySource")

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))

process.treeExtractor = cms.EDAnalyzer("SingleTreeExtractor",
    inputFile = cms.untracked.string("/afs/hep.wisc.edu/user/pkhalili2/ExoHiggs/CMSSW_14_0_15/src/MyAnalysis/AK8FlatTreeProducer/test/ak15_test.root"),
    outputFile = cms.untracked.string("/afs/hep.wisc.edu/user/pkhalili2/ExoHiggs/CMSSW_14_0_15/src/MyAnalysis/AK8FlatTreeProducer/test/CleanBack_ak15_test.root")
)

process.p = cms.Path(process.treeExtractor)
