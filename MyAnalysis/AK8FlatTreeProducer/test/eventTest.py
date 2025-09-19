import uproot

filename = "/hdfs/store/user/abdollah/exoHiggs_8b_Final_MiniAOD_Updated.root"

with uproot.open(filename) as file:
    if "Events" in file:
        events_tree = file["Events"]
        num_events = events_tree.num_entries
        print(f"Total number of events in the 'Events' tree: {num_events}")
    else:
        print("No 'Events' tree found in the ROOT file.")
