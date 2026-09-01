// $Id$
//
//    File: JEventProcessor_photon_discrim7.h
// Created: Thu Sep 11 02:38:30 PM EDT 2025
// Creator: debiase (on Linux ifarm2402.jlab.org 5.14.0-570.33.2.el9_6.x86_64 x86_64)
//

/// For more information on the syntax changes between JANA1 and JANA2, visit: https://jeffersonlab.github.io/JANA2/#/jana1to2/jana1-to-jana2

#ifndef _DEventProcessor_FCALextract_
#define _DEventProcessor_FCALextract_

#include <pthread.h>

#include <TFile.h>
#include <TTree.h>
#include <TMath.h>

#include <JANA/JFactory.h>
#include <JANA/JEventProcessor.h>
#include <TRACKING/DReferenceTrajectory.h>
// #include <JANA/Services/JLockService.h> // Required for accessing services

class DEventProcessor_FCALextract:public JEventProcessor{
    public:
        DEventProcessor_FCALextract();
        ~DEventProcessor_FCALextract();
        const char* className(void){return "DEventProcessor_FCALextract";}
        
        TTree *fcal_track_tree;

    private:
    	
    const Double_t FCAL_face_z = 625.0;
	const Double_t target_center_z = 65.0;
	const Double_t delta_theta_thresh = 0.35*(TMath::Pi()/180.0);
	const Double_t delta_phi_thresh = 3.35*(TMath::Pi()/180.0);
	const Double_t delta_t_thresh = 2.0; // Find some reasonable time to set this to, or don't use it
	const Double_t distance_thresh = 2.0; // 1 FCAL2 block, half an FCAL1 block
	//const Double_t distance_thresh = 3.44/2; // See below:
		// Theta and phi thresholds from https://halldweb.jlab.org/DocDB/0040/004025/011/fcal_bcal_efficiency.pdf for throwns at theta = 6 deg
		// Approximate ranges in theta and phi as distance using arc length
		// delta_theta_thresh * (FCAL_face_z - target_center_z) ~ 3.42 cm
		// delta_phi_thresh * (FCAL_face_z - target_center_z) * tan(6.0*(TMath::Pi()/180.0)) ~ 3.44 cm
		// 3.44 cm is the full range, use 3.44 / 2 cm as the radius of a circle around the point
	
	//members correspond to branches of tree
        //fcal_track_tree tree
	Int_t m_eventnumber;
	Int_t m_showernumber;
	
	Float_t m_recon_FCAL_x;
    Float_t m_recon_FCAL_y;
    Float_t m_recon_FCAL_z;
    Float_t m_recon_FCAL_r;
    Float_t m_recon_FCAL_phi;
	Float_t m_recon_FCAL_theta;
    Float_t m_recon_FCAL_E;
    Float_t m_recon_FCAL_t;
    Float_t m_recon_FCAL_dt;
	Float_t m_recon_FCAL_beta;
	Float_t m_recon_FCAL_E1E9;
	Float_t m_recon_FCAL_E9E25;
	Float_t m_recon_FCAL_sumU;
	Float_t m_recon_FCAL_sumV;
	Float_t m_recon_FCAL_asymUV;
	Float_t m_recon_FCAL_docaTr;
    Float_t m_recon_FCAL_tproj;
	Float_t m_recon_FCAL_XWidth;
	Float_t m_recon_FCAL_YWidth;
	Float_t m_recon_FCAL_RWidth;
	Float_t m_recon_FCAL_TWidth;
	Int_t m_recon_FCAL_nHits;
	Float_t m_recon_FCAL_quality;

    std::vector<int>   fcal_row;
    std::vector<int>   fcal_column;
    std::vector<float> fcal_x;
    std::vector<float> fcal_y;
    std::vector<float> fcal_E;
    std::vector<float> fcal_t;

    Int_t m_n_matched; // number of throwns a reconstructed matches to (geometrically, temporally, and energetically)
    Bool_t m_recon_FCAL_match_thrown;
    Int_t m_thrown_ptype;
    Int_t m_thrown_charge;
    Int_t m_matched_MCTrackID;
    Float_t m_match_px;
    Float_t m_match_py;
    Float_t m_match_pz;
    Float_t m_match_phi;
    Float_t m_match_theta;
    Float_t m_match_E;
    Float_t m_match_t;
	
	Int_t m_n_merged; // number of throwns a reconstructed matches to (geometrically only)
	
	int loceventnumber = 0;
	int locshowernumber;
	
	const DMagneticFieldMap *bfield;
	
        void Init() override;                       ///< Called once at program start.
        void BeginRun(const std::shared_ptr<const JEvent>& event) override; ///< Called everytime a new run number is detected.
        void Process(const std::shared_ptr<const JEvent>& event) override;  ///< Called every event.
        void EndRun() override;                     ///< Called everytime run number changes, provided BeginRun has been called.
       	void Finish() override;                     ///< Called after last event of last event source has been processed.
        
        pthread_mutex_t mutex;

    	// std::shared_ptr<JLockService> lockService; //Used to access all the services, its value should be set inside Init()
};

#endif // _DEventProcessor_FCALextract_
