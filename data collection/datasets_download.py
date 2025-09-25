import uproot
import awkward as ak

# Read file names from 'file1.txt'
with open("file1.txt", "r") as f:
    root_files = [line.strip() for line in f if line.strip()]

# List of branches to read
branches = [
    "nMuon", "Muon_pt", "Muon_eta", "Muon_phi", "Muon_mass",
    "Muon_charge", "Muon_looseId", "Muon_mediumId", "Muon_tightId",
    "Muon_pfRelIso03_all", "Muon_pfRelIso04_all", "Muon_dxy",
    "Muon_dz", "Muon_pdgId", "Muon_ip3d", "Muon_sip3d", "Muon_genPartIdx",
    "PuppiMET_phi", "PuppiMET_pt",
    "Jet_eta", "Jet_phi", "Jet_pt", "Jet_mass", "Jet_jetId", "Jet_hadronFlavour",
    "Jet_btagDeepFlavB", "Jet_btagDeepFlavCvB", "Jet_btagDeepFlavCvL", "Jet_btagDeepFlavQG",
    "PV_npvs"
]

# Loop through all files
for idx, file_url in enumerate(root_files, start=1):
    try:
        print(f"Processing file {idx}: {file_url}")
        infile = uproot.open(file_url, timeout=1400)
        events = infile["Events"]

        # Read required branches
        data = events.arrays(branches)

        # Apply condition: nMuon >= 1
        mask = data["nMuon"] >= 1
        filtered_data = data[mask]

        # Write to new ROOT file
        output_name = f"tempsignalfinder{idx}.root"
        with uproot.recreate(output_name) as new_file:
            new_file["Events"] = {key: filtered_data[key] for key in filtered_data.fields}

        print(f"Saved to {output_name}\n")

    except Exception as e:
        print(f"Error processing file {file_url}: {e}\n")
