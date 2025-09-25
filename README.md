## Introduction
The CMS (Compact Muon Solenoid) experiment at CERN’s Large Hadron Collider (LHC) is one of the largest and most complex particle detectors in the world. It investigates the fundamental building blocks of matter by studying high-energy proton-proton collisions. The LHC allows physicists to explore rare processes and test the predictions of the Standard Model of particle physics.  
One of the key studies at CMS is the production of W boson pairs (WW), which can provide insights into electroweak interactions, Higgs boson properties, and potential signals of new physics beyond the Standard Model. This project focuses on analyzing WW events using simulated and experimental data, applying selection cuts to isolate signal events from background noise.

## Objective
The main goal of this project is to study WW production events in CMS data and understand how selection criteria affect the identification of signal events. By applying specific kinematic cuts, we aim to reduce background noise from other processes and extract meaningful distributions that can be compared with theoretical predictions. This analysis helps in gaining practical experience with particle physics data, Docker-based reproducible environments, and ROOT/Python tools.

## Tools & Environment
- **Docker** for a reproducible analysis environment  
- **ROOT** and **Python** for data processing and plotting  
- **Dataset:** CMS WW simulation / experimental data  

## Workflow
1. **Set up the analysis environment**  
   - Use Docker to create a reproducible workspace with ROOT, Python, and necessary libraries installed.  

2. **Load and explore the dataset**  
   - Examine the structure of the CMS WW data, identify key variables (e.g., lepton pT, MET), and understand the event types.  

3. **Apply selection cuts**  
   - Implement cuts on kinematic variables to reduce background and enhance signal purity.  
   - Experiment with different combinations of cuts to study their effect on event selection.  

4. **Generate plots and distributions**  
   - Compare distributions before and after applying selection cuts to evaluate the effectiveness of the analysis.  

## Conclusion
The analysis workflow successfully isolated WW events and demonstrated the impact of different selection cuts on the signal-to-background ratio.
