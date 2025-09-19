import uproot
import awkward as ak
import numpy as np

# Open the ROOT file and read the tree
file = uproot.open("/afs/hep.wisc.edu/user/pkhalili2/ExoHiggs/CMSSW_14_0_15/src/MyAnalysis/AK8FlatTreeProducer/test/ak15_Extract.root")
tree = file["Events"]
data = tree.arrays(library="ak")

# Split proportions
n = len(data)
indices = np.arange(n)
np.random.seed(42)
np.random.shuffle(indices)

n_train = int(n * 0.8)
n_val = int(n * 0.1)

train_idx = indices[:n_train]
val_idx = indices[n_train:n_train + n_val]
test_idx = indices[n_train + n_val:]

train_data = data[train_idx]
val_data = data[val_idx]
test_data = data[test_idx]

# Save split files
with uproot.recreate("train.root") as f:
    f["tree"] = train_data

with uproot.recreate("val.root") as f:
    f["tree"] = val_data

with uproot.recreate("test.root") as f:
    f["tree"] = test_data
