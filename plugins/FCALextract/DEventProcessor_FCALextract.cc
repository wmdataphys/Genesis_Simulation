// $Id$
//
//    File: DEventProcessor_FCALextract.cc
// Created: Thu Sep 11 02:38:30 PM EDT 2025
// Creator: debiase (on Linux ifarm2402.jlab.org 5.14.0-570.33.2.el9_6.x86_64 x86_64)
//

/// For more information on the syntax changes between JANA1 and JANA2, visit: https://jeffersonlab.github.io/JANA2/#/jana1to2/jana1-to-jana2
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

#include <TLorentzVector.h>

#include <JANA/JApplication.h>
#include <particleType.h>
#include <DANA/DEvent.h>
#include <TRACKING/DMCThrown.h>
#include <TRACKING/DTrackTimeBased.h>
#include <PID/DNeutralShower.h>
#include <FCAL/DFCALShower.h>
#include <FCAL/DFCALCluster.h>
#include <FCAL/DFCALGeometry.h>
#include <FCAL/DFCALHit.h>
#include <PID/DEventRFBunch.h>
#include <units.h>

#include "DEventProcessor_FCALextract.h"


// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
extern "C"{
void InitPlugin(JApplication *app){
    InitJANAPlugin(app);
    app->Add(new DEventProcessor_FCALextract());
}
} // "C"


//------------------
// DEventProcessor_FCALextract (Constructor)
//------------------
DEventProcessor_FCALextract::DEventProcessor_FCALextract()
{
	SetTypeName(NAME_OF_THIS); // Provide JANA with this class's name

    // Parameters and Services should be accessed from Init() instead of here!
}

//------------------
// ~DEventProcessor_FCALextract (Destructor)
//------------------
DEventProcessor_FCALextract::~DEventProcessor_FCALextract()
{	
}

//------------------
// Init
//------------------
void DEventProcessor_FCALextract::Init()
{
	// This is called once at program startup.
	//auto app = GetApplication();
	// lockService should be initialized here like this
	// lockService = app->GetService<JLockService>();
	fcal_track_tree = new TTree("fcal_track_tree","fcal_track_tree");
	
	fcal_track_tree->Branch("eventnumber",&m_eventnumber,"eventnumber/I");
	fcal_track_tree->Branch("showernumber",&m_showernumber,"showernumber/I");
	fcal_track_tree->Branch("recon_FCAL_x", &m_recon_FCAL_x, "recon_FCAL_x/F");
	fcal_track_tree->Branch("recon_FCAL_y", &m_recon_FCAL_y, "recon_FCAL_y/F");
	fcal_track_tree->Branch("recon_FCAL_z", &m_recon_FCAL_z, "recon_FCAL_z/F");
	fcal_track_tree->Branch("recon_FCAL_r", &m_recon_FCAL_r, "recon_FCAL_r/F");
	fcal_track_tree->Branch("recon_FCAL_phi", &m_recon_FCAL_phi, "recon_FCAL_phi/F");
	fcal_track_tree->Branch("recon_FCAL_theta", &m_recon_FCAL_theta, "recon_FCAL_theta/F");
	fcal_track_tree->Branch("recon_FCAL_E", &m_recon_FCAL_E, "recon_FCAL_E/F");
	fcal_track_tree->Branch("recon_FCAL_t", &m_recon_FCAL_t, "recon_FCAL_t/F");
	fcal_track_tree->Branch("recon_FCAL_dt", &m_recon_FCAL_dt, "recon_FCAL_dt/F");
	fcal_track_tree->Branch("recon_FCAL_beta", &m_recon_FCAL_beta, "recon_FCAL_beta/F");
	fcal_track_tree->Branch("recon_FCAL_E1E9", &m_recon_FCAL_E1E9, "recon_FCAL_E1E9/F");
	fcal_track_tree->Branch("recon_FCAL_E9E25", &m_recon_FCAL_E9E25, "recon_FCAL_E9E25/F");
	fcal_track_tree->Branch("recon_FCAL_sumU", &m_recon_FCAL_sumU, "recon_FCAL_sumU/F");
	fcal_track_tree->Branch("recon_FCAL_sumV", &m_recon_FCAL_sumV, "recon_FCAL_sumV/F");
	fcal_track_tree->Branch("recon_FCAL_asymUV", &m_recon_FCAL_asymUV, "recon_FCAL_asymUV/F");
	fcal_track_tree->Branch("recon_FCAL_docaTr", &m_recon_FCAL_docaTr, "recon_FCAL_docaTr/F");
	fcal_track_tree->Branch("recon_FCAL_tproj",&m_recon_FCAL_tproj,"recon_FCAL_tproj/F");
	fcal_track_tree->Branch("recon_FCAL_XWidth",&m_recon_FCAL_XWidth,"recon_FCAL_XWidth/F");
	fcal_track_tree->Branch("recon_FCAL_YWidth",&m_recon_FCAL_YWidth,"recon_FCAL_YWidth/F");
	fcal_track_tree->Branch("recon_FCAL_RWidth",&m_recon_FCAL_RWidth,"recon_FCAL_RWidth/F");
	fcal_track_tree->Branch("recon_FCAL_TWidth",&m_recon_FCAL_TWidth,"recon_FCAL_TWidth/F");
	fcal_track_tree->Branch("recon_FCAL_nHits", &m_recon_FCAL_nHits, "recon_FCAL_nHits/I");
	fcal_track_tree->Branch("n_matched",&m_n_matched,"n_matched/I"); //Number of matched throwns for a single neutral
	fcal_track_tree->Branch("recon_FCAL_match_thrown",&m_recon_FCAL_match_thrown,"recon_FCAL_match_thrown/O");
	fcal_track_tree->Branch("thrown_ptype",&m_thrown_ptype,"thrown_ptype/I");
	fcal_track_tree->Branch("thrown_charge",&m_thrown_charge,"thrown_charge/I");
	fcal_track_tree->Branch("matched_MCTrackID",&m_matched_MCTrackID,"matched_MCTrackID/I");
	fcal_track_tree->Branch("match_px",&m_match_px,"match_px/F");
	fcal_track_tree->Branch("match_py",&m_match_py,"match_py/F");
	fcal_track_tree->Branch("match_pz",&m_match_pz,"match_pz/F");
	fcal_track_tree->Branch("match_phi",&m_match_phi,"match_phi/F");
	fcal_track_tree->Branch("match_theta",&m_match_theta,"match_theta/F");
	fcal_track_tree->Branch("match_E",&m_match_E,"match_E/F");
	fcal_track_tree->Branch("match_t",&m_match_t,"match_t/F");
	fcal_track_tree->Branch("m_n_merged",&m_n_merged,"merged/I");

    fcal_track_tree->Branch("EventID", &m_eventnumber, "EventID/I");
    fcal_track_tree->Branch("NHitsFCAL", &m_recon_FCAL_nHits, "NHitsFCAL/I");
    fcal_track_tree->Branch("fcal_row", &fcal_row);
    fcal_track_tree->Branch("fcal_column", &fcal_column);
    fcal_track_tree->Branch("fcal_x", &fcal_x);
    fcal_track_tree->Branch("fcal_y", &fcal_y);
    fcal_track_tree->Branch("fcal_E", &fcal_E);
    fcal_track_tree->Branch("fcal_t", &fcal_t);
	
	pthread_mutex_init(&mutex, NULL);
}

//------------------
// BeginRun
//------------------
void DEventProcessor_FCALextract::BeginRun(const std::shared_ptr<const JEvent> &event)
{
    // This is called whenever the run number changes
    auto locEvent = event;
    bfield = DEvent::GetBfield(locEvent);
}

//------------------
// Process
//------------------
void DEventProcessor_FCALextract::Process(const std::shared_ptr<const JEvent> &event)
{
	// This is called for every event. Use of common resources like writing
	// to a file or filling a histogram should be mutex protected. Using
	// event->Get(...) to get reconstructed objects (and thereby activating the
	// reconstruction algorithm) should be done outside of any mutex lock
	// since multiple threads may call this method at the same time.
	// Here's an example:
	//
	// vector<const MyDataClass*> mydataclasses;
	// event->Get(mydataclasses);
	//
	// If you have lockService initialized in Init() then you can acquire locks like this
	// lockService->RootFillLock(this);
	//  ... fill historgrams or trees ...
	// lockService->RootFillUnLock(this);
	auto locEvent = event;
	m_eventnumber = static_cast<Int_t>(locEvent->GetEventNumber());
	//
	vector<const DMCThrown*> throwns;
	//vector<const DNeutralShower*> Neutral_showers; //REMOVED
	const DFCALShower* FCAL_shower; // REMAINS
	vector<const DFCALShower*> locFCALShowers; //UPDATED
	vector<const DEventRFBunch*> RFBunch;

	locEvent->Get(throwns,"FinalState");
	//locEvent->Get(Neutral_showers); //REMOVED
	locEvent->Get(locFCALShowers); //UPDATED
	locEvent->Get(RFBunch);
	double rftime = RFBunch[0]->dTime;
	
	// Fill histograms below here
	pthread_mutex_lock(&mutex);
	
	locshowernumber = 0;

    fcal_row.clear();
    fcal_column.clear();
    fcal_x.clear();
    fcal_y.clear();
    fcal_E.clear();
    fcal_t.clear();
	
	for (int i=0; i<(int)locFCALShowers.size(); i++) {
		//if (Neutral_showers[i]->dDetectorSystem != SYS_FCAL) continue; //REMOVED
		FCAL_shower = locFCALShowers[i];
		
		m_n_matched=0;
		m_n_merged=0;
		m_matched_MCTrackID = -1;
		
		//m_recon_FCAL_quality = Neutral_showers[i]->dQuality; //REMOVED
		m_showernumber = locshowernumber;
		locshowernumber++;
		
		double x = FCAL_shower->getPosition().X();
		double y = FCAL_shower->getPosition().Y();
		double z = FCAL_shower->getPosition().Z();
		double r = sqrt(x*x + y*y);
		double t = FCAL_shower->getTime();
		double phi = atan2(y,x);
		double theta = atan2(r,z-target_center_z);
		m_recon_FCAL_x = x;
		m_recon_FCAL_y = y;
		m_recon_FCAL_z = z;
		m_recon_FCAL_r = r;
		m_recon_FCAL_phi = phi;
		m_recon_FCAL_theta = theta;
		m_recon_FCAL_E = FCAL_shower->getEnergy();
		m_recon_FCAL_t = t;

		TVector3 flightDistance_vector(x,y,z-target_center_z);
		double flightDistance = flightDistance_vector.Mag();
		double flightTime = t - rftime; 
		m_recon_FCAL_dt = flightTime - FCAL_shower->getTimeTrack();
		m_recon_FCAL_beta = flightDistance / flightTime; 
		m_recon_FCAL_E1E9 = FCAL_shower->getE1E9();
		m_recon_FCAL_E9E25 = FCAL_shower->getE9E25();
		m_recon_FCAL_sumU = FCAL_shower->getSumU();
		m_recon_FCAL_sumV = FCAL_shower->getSumV();
		m_recon_FCAL_asymUV = ((m_recon_FCAL_sumU - m_recon_FCAL_sumV)/(m_recon_FCAL_sumU + m_recon_FCAL_sumV));
		m_recon_FCAL_docaTr = FCAL_shower->getDocaTrack();

		m_recon_FCAL_tproj = flightTime - flightDistance/SPEED_OF_LIGHT; // Serves the same purpose as beta

		// Shower shape widths
		m_recon_FCAL_XWidth=0;
		m_recon_FCAL_YWidth=0;
		m_recon_FCAL_RWidth=0;
		m_recon_FCAL_TWidth=0;
		m_recon_FCAL_nHits=0;
		double ax = 0;
		double ay = 0;
		double ar = 0;
		double at = 0;
		double c = 0;
		
		vector<const DFCALCluster*> clusters;
		FCAL_shower->Get(clusters);

        fcal_row.clear();
        fcal_column.clear();
        fcal_x.clear();
        fcal_y.clear();
        fcal_E.clear();
        fcal_t.clear();

		for(unsigned int j=0; j<clusters.size(); j++){
		
			vector<const DFCALHit*> hits;
			clusters[j]->Get(hits);
			for(unsigned int k=0; k<hits.size(); k++){

				double E, X, Y, R, T;

				E = hits[k]->E;
				X = hits[k]->x;
				Y = hits[k]->y;
				R = sqrt(X*X + Y*Y);
				T = hits[k]->t;

                fcal_row.push_back(hits[k]->row);
                fcal_column.push_back(hits[k]->column);
                fcal_x.push_back(X);
                fcal_y.push_back(Y);
                fcal_E.push_back(E);
                fcal_t.push_back(T);

				double delx = X - x;
				double dely = Y - y;
				double delr = R - r;
				double delt = T - t;

				ax += E*delx*delx;
				ay += E*dely*dely;
				ar += E*delr*delr;
				at += E*delt*delt;

				m_recon_FCAL_nHits++;
				c += E;
			}
		}

		m_recon_FCAL_XWidth = sqrt((ax/c));
		m_recon_FCAL_YWidth = sqrt((ay/c));
		m_recon_FCAL_RWidth = sqrt((ar/c));
		m_recon_FCAL_TWidth = sqrt((at/c));
		
		// Check if this matches any of the thrown particles
		m_recon_FCAL_match_thrown = false; // Initialize to false, set to true if any throwns match the shower
		m_thrown_ptype = 0;
		m_thrown_charge = 3;
		double thrown_phi, thrown_theta, thrown_E, thrown_t, x1, y1, z1;
		for (int j=0; j<(int)throwns.size(); j++) {
			thrown_phi=7.0;
			thrown_theta=7.0;
			thrown_E=0.0;
			thrown_t=0.0;
			if (throwns[j]->charge() == 0) { //Neutral matching
			double x0 = throwns[j]->x();
			double y0 = throwns[j]->y();
			double z0 = throwns[j]->z();
			double t0 = throwns[j]->time();
			double px0 = throwns[j]->px();
			double py0 = throwns[j]->py();
			double pz0 = throwns[j]->pz();

			double parameter = (z - z0)/pz0;
			x1 = x0 + parameter*px0;
			y1 = y0 + parameter*py0;
			z1 = z0 + parameter*pz0;
			thrown_phi = atan2(y1,x1);
			thrown_theta = atan2(sqrt(x1*x1 + y1*y1),z1-target_center_z);
			thrown_E = throwns[j]->energy();
			double temp_beta = throwns[j]->pmag() / sqrt(throwns[j]->mass()*throwns[j]->mass() + throwns[j]->pmag()*throwns[j]->pmag());
			thrown_t = t0 + sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0) + (z1 - z0)*(z1 - z0))/(temp_beta*SPEED_OF_LIGHT);
			}
			else { //Charged matching
			DReferenceTrajectory rt(bfield);
			rt.SetMass(throwns[j]->mass());
			rt.q = throwns[j]->charge();
			rt.Swim(throwns[j]->position(),throwns[j]->momentum(),rt.q);
			DVector3 projPos;
			DVector3 origin(0.0,0.0,z);
			DVector3 norm(0.0,0.0,1.0);
			double s,t;

			if (rt.GetIntersectionWithPlane(origin,norm,projPos,&s,&t) != NOERROR) continue;

			x1 = projPos.X();
			y1 = projPos.Y();
			z1 = projPos.Z();
			thrown_phi = atan2(y1,x1);
			thrown_theta = atan2(sqrt(x1*x1 + y1*y1),z1-target_center_z);
			thrown_E = throwns[j]->energy();
			thrown_t = throwns[j]->time() + t;
			}

			double delta_E = m_recon_FCAL_E - thrown_E;
			double sig_E_NIM = thrown_E*sqrt(.062*.062/thrown_E + .047*.047); // FIND NUMBERS FOR THIS
			double delta_E_thresh = min(4.0*sig_E_NIM,thrown_E);

			double delta_t = m_recon_FCAL_t - thrown_t;

			double distance = sqrt((x1 - x)*(x1 - x) + (y1 - y)*(y1 - y));

			bool temp_matched = false;

			// Check for a match
			if (throwns[j]->PID() == 1) { // Geometrically, temporally, and energetically for photons
			if (fabs(delta_E) < delta_E_thresh && distance < distance_thresh && fabs(delta_t) < delta_t_thresh) temp_matched = true;
			}
			else { // Geometrically and temporally for other particles
			if (distance < 2*distance_thresh && fabs(delta_t) < delta_t_thresh) temp_matched = true;
			}

			if (temp_matched) {
			m_recon_FCAL_match_thrown = true;

			m_match_E = throwns[j]->energy();
			m_match_px = throwns[j]->px(); // MAYBE REMOVE MATCH VARIABLES.
			m_match_py = throwns[j]->py();
			m_match_pz = throwns[j]->pz();
			m_match_phi = thrown_phi;
			m_match_theta = thrown_theta;
			m_match_t = thrown_t;

            // convert to PDG 
			m_thrown_ptype = PDGtype(throwns[j]->PID());
			m_thrown_charge = throwns[j]->charge();
			m_matched_MCTrackID = throwns[j]->myid;
			m_n_matched++;
			}

			// Check for a match (only geometrically)
			if (throwns[j]->PID() == 1 && distance < distance_thresh) m_n_merged++;
			else if (throwns[j]->PID() != 1 && distance < 2*distance_thresh) m_n_merged++;
		}
		
		fcal_track_tree->Fill();
	}
	
	pthread_mutex_unlock(&mutex);
}

//------------------
// EndRun
//------------------
void DEventProcessor_FCALextract::EndRun()
{
    // This is called whenever the run number changes, before it is
    // changed to give you a chance to clean up before processing
    // events from the next run number.
}

//------------------
// Finish
//------------------
void DEventProcessor_FCALextract::Finish()
{
    // Called before program exit after event processing is finished.
}
