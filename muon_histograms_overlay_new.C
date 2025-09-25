#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLorentzVector.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TString.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

// Δφ calculation
float deltaPhi(float phi1, float phi2) {
    float dphi = phi1 - phi2;
    while (dphi > M_PI)  dphi -= 2 * M_PI;
    while (dphi <= -M_PI) dphi += 2 * M_PI;
    return dphi;
}

void processMuonFile(TTree* tree,
                     TH1D* h_leading_pt, TH1D* h_subleading_pt,
                     TH1D* h_leading_eta, TH1D* h_subleading_eta,
                     TH1D* h_mll, TH1D* h_dphi_ll,
                     TH1D* h_dphi_met_lep, TH1D* h_jet_pt, TH1D* h_met_pt,
                     TH1D* h_nmuon)   // <-- new histogram
{
    // Branches
    UInt_t nMuon;
    Float_t Muon_pt[100], Muon_eta[100], Muon_phi[100], Muon_mass[100], Muon_pfRelIso03_all[100];
    Int_t Muon_charge[100];
    Bool_t Muon_tightId[100];
    Float_t Jet_pt[100];
    Float_t PuppiMET_pt, PuppiMET_phi;

    tree->SetBranchAddress("nMuon", &nMuon);
    tree->SetBranchAddress("Muon_pt", Muon_pt);
    tree->SetBranchAddress("Muon_eta", Muon_eta);
    tree->SetBranchAddress("Muon_phi", Muon_phi);
    tree->SetBranchAddress("Muon_mass", Muon_mass);
    tree->SetBranchAddress("Muon_charge", Muon_charge);
    tree->SetBranchAddress("Muon_tightId", Muon_tightId);
    tree->SetBranchAddress("Muon_pfRelIso03_all", Muon_pfRelIso03_all);
    tree->SetBranchAddress("PuppiMET_pt", &PuppiMET_pt);
    tree->SetBranchAddress("PuppiMET_phi", &PuppiMET_phi);
    tree->SetBranchAddress("Jet_pt", Jet_pt);

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);



        // Muon selection
        if (nMuon != 2) continue;
        if (Muon_charge[0] * Muon_charge[1] != -1) continue;
        if (!(Muon_tightId[0] && Muon_tightId[1])) continue;
        if (!(Muon_pfRelIso03_all[0] < 0.15 && Muon_pfRelIso03_all[1] < 0.15)) continue;
        if (!(Muon_pt[0] > 20 && Muon_pt[1] > 20)) continue;
        if (!(fabs(Muon_eta[0]) < 2.4 && fabs(Muon_eta[1]) < 2.4)) continue;
        if (Muon_pt[0] <= 25) continue;

        // Invariant mass
        TLorentzVector mu1, mu2;
        mu1.SetPtEtaPhiM(Muon_pt[0], Muon_eta[0], Muon_phi[0], Muon_mass[0]);
        mu2.SetPtEtaPhiM(Muon_pt[1], Muon_eta[1], Muon_phi[1], Muon_mass[1]);
        float mll = (mu1 + mu2).M();

        // Δφ between muons
        float dphi_ll = fabs(deltaPhi(Muon_phi[0], Muon_phi[1]));

        //  Extra Cuts 
        if (!(dphi_ll > 2.5)) continue;              // Δφ > 2.5
        if (!(PuppiMET_pt > 0 && PuppiMET_pt < 25)) continue; // 0 < MET < 25
        if (!(mll > 80 && mll < 100)) continue;      // 80 < MLL < 100

        // Fill muon histograms
        h_leading_pt->Fill(Muon_pt[0]);
        h_subleading_pt->Fill(Muon_pt[1]);
        h_leading_eta->Fill(Muon_eta[0]);
        h_subleading_eta->Fill(Muon_eta[1]);
        
        // Fill nMuon BEFORE any cut
        h_nmuon->Fill(nMuon);

        h_mll->Fill(mll);
        h_dphi_ll->Fill(dphi_ll);

        // Δφ between MET and closest muon
        float dphi_met_mu0 = fabs(deltaPhi(Muon_phi[0], PuppiMET_phi));
        float dphi_met_mu1 = fabs(deltaPhi(Muon_phi[1], PuppiMET_phi));
        h_dphi_met_lep->Fill((dphi_met_mu0 < dphi_met_mu1) ? dphi_met_mu0 : dphi_met_mu1);

        // Jet pT (no cuts)
        for (int j = 0; j < 100; j++) {
            if (Jet_pt[j] <= 0) break;
            h_jet_pt->Fill(Jet_pt[j]);
        }

        // MET pT (after cut, but still filled for hist)
        h_met_pt->Fill(PuppiMET_pt);
    }
}

void muon_histograms_overlay_new() {
    // Read file paths
    ifstream infile("file.txt");
    vector<string> files;
    string line;
    while (getline(infile, line)) {
        if (!line.empty()) files.push_back(line);
    }

    vector<int> colors = {kRed, kBlue, kGreen+2, kMagenta, kOrange+1, kCyan+1, kPink+1, kAzure+2, kViolet+1, kGray+2};

    // Hist collections
    vector<TH1D*> h_leading_pt_all, h_subleading_pt_all, h_leading_eta_all, h_subleading_eta_all,
                  h_mll_all, h_dphi_ll_all, h_dphi_met_lep_all, h_jet_pt_all, h_met_pt_all, h_nmuon_all;

    TLegend *leg = new TLegend(0.75, 0.65, 0.95, 0.88);

    for (size_t i = 0; i < files.size(); i++) {
        // Short label
        TString fname(files[i]);
        TString shortname = gSystem->BaseName(fname); // remove path
        shortname.ReplaceAll(".root", ""); // remove extension

        // Open file
        TFile *file = TFile::Open(files[i].c_str());
        if (!file || file->IsZombie()) {
            cerr << "Cannot open " << files[i] << endl;
            continue;
        }
        TTree *tree = (TTree*)file->Get("Events");
        if (!tree) {
            cerr << "No Events tree in " << files[i] << endl;
            continue;
        }

        // Create hists for this file
        TH1D *h_leading_pt      = new TH1D(Form("h_leading_pt_%zu", i), "Leading Muon pT; p_{T} [GeV]; Arbitrary Units", 80, 0, 200);
        TH1D *h_subleading_pt   = new TH1D(Form("h_subleading_pt_%zu", i), "Subleading Muon pT; p_{T} [GeV]; Arbitrary Units", 80, 0, 200);
        TH1D *h_leading_eta     = new TH1D(Form("h_leading_eta_%zu", i), "Leading Muon #eta; #eta; Arbitrary Units", 50, -2.5, 2.5);
        TH1D *h_subleading_eta  = new TH1D(Form("h_subleading_eta_%zu", i), "Subleading Muon #eta; #eta; Arbitrary Units", 50, -2.5, 2.5);
        TH1D *h_mll             = new TH1D(Form("h_mll_%zu", i), "Dilepton Invariant Mass; m_{#mu#mu} [GeV]; Arbitrary Units", 15, 80, 100);
        TH1D *h_dphi_ll         = new TH1D(Form("h_dphi_ll_%zu", i), "#Delta#phi(#mu_{1},#mu_{2}); #Delta#phi; Arbitrary Units", 25, 2.5, M_PI);
        TH1D *h_dphi_met_lep    = new TH1D(Form("h_dphi_met_lep_%zu", i), "#Delta#phi(MET, closest muon); #Delta#phi; Arbitrary Units", 25, 0, M_PI);
        TH1D *h_jet_pt          = new TH1D(Form("h_jet_pt_%zu", i), "Jet pT (no cuts); p_{T} [GeV]; Arbitrary Units", 80, 0, 200);
        TH1D *h_met_pt          = new TH1D(Form("h_met_pt_%zu", i), "MET pT (no cuts); MET [GeV]; Arbitrary Units", 25, 0, 50);
        TH1D *h_nmuon           = new TH1D(Form("h_nmuon_%zu", i), "Muon Multiplicity; N_{#mu}; Arbitrary Units", 10, 0, 10);

        // Color
        int col = colors[i % colors.size()];
        for (auto* h : {h_leading_pt, h_subleading_pt, h_leading_eta, h_subleading_eta,
                        h_mll, h_dphi_ll, h_dphi_met_lep, h_jet_pt, h_met_pt, h_nmuon}) {
            h->SetLineColor(col);
            h->SetLineWidth(2);
        }

        processMuonFile(tree, h_leading_pt, h_subleading_pt, h_leading_eta, h_subleading_eta,
                        h_mll, h_dphi_ll, h_dphi_met_lep, h_jet_pt, h_met_pt, h_nmuon);

        // --- Normalize histograms to unity ---
        for (auto* h : {h_leading_pt, h_subleading_pt, h_leading_eta, h_subleading_eta,
                        h_mll, h_dphi_ll, h_dphi_met_lep, h_jet_pt, h_met_pt, h_nmuon}) {
            double integral = h->Integral();
            if (integral > 0) h->Scale(1.0 / integral);
        }

        h_leading_pt_all.push_back(h_leading_pt);
        h_subleading_pt_all.push_back(h_subleading_pt);
        h_leading_eta_all.push_back(h_leading_eta);
        h_subleading_eta_all.push_back(h_subleading_eta);
        h_mll_all.push_back(h_mll);
        h_dphi_ll_all.push_back(h_dphi_ll);
        h_dphi_met_lep_all.push_back(h_dphi_met_lep);
        h_jet_pt_all.push_back(h_jet_pt);
        h_met_pt_all.push_back(h_met_pt);
        h_nmuon_all.push_back(h_nmuon);

        leg->AddEntry(h_leading_pt, shortname, "l");
    }

    auto drawOverlay = [&](vector<TH1D*>& hists, const char* cname, const char* title) {
        TCanvas *c = new TCanvas(cname, title, 1200, 900);
        gStyle->SetOptStat(0);
        for (size_t i = 0; i < hists.size(); i++) {
            if (i == 0) hists[i]->Draw("HIST");
            else hists[i]->Draw("HIST SAME");
        }
        leg->Draw();
        c->SaveAs(Form("%s.png", cname));
    };

    drawOverlay(h_leading_pt_all, "leading_pt_overlay", "Leading Muon pT");
    drawOverlay(h_subleading_pt_all, "subleading_pt_overlay", "Subleading Muon pT");
    drawOverlay(h_leading_eta_all, "leading_eta_overlay", "Leading Muon Eta");
    drawOverlay(h_subleading_eta_all, "subleading_eta_overlay", "Subleading Muon Eta");
    drawOverlay(h_mll_all, "mll_overlay", "Dilepton Mass");
    drawOverlay(h_dphi_ll_all, "dphi_ll_overlay", "DeltaPhi between Muons");
    drawOverlay(h_dphi_met_lep_all, "dphi_met_lep_overlay", "DeltaPhi MET vs closest muon");
    drawOverlay(h_jet_pt_all, "jet_pt_overlay", "Jet pT");
    drawOverlay(h_met_pt_all, "met_pt_overlay", "MET pT");
    drawOverlay(h_nmuon_all, "nmuon_overlay", "Muon Multiplicity"); // <-- new plot
}
