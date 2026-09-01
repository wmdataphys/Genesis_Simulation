#include "DEventProcessor_TrackMLextract.h"
#include <iostream>
#include <JANA/JApplication.h>
#include <JANA/JEvent.h>
#include <TRACKING/DTrackTimeBased.h>
#include <FDC/DFDCPseudo.h>
#include <FDC/DFDCWire.h>
#include <particleType.h>
#include <TFile.h>
#include <TRACKING/DMCThrown.h>
#include <TRACKING/DMCTrackHit.h>
#include <TDirectory.h>

using namespace std;

extern "C"{
void InitPlugin(JApplication *app){
    InitJANAPlugin(app);
    app->Add(new DEventProcessor_TrackMLextract());
}
}

DEventProcessor_TrackMLextract::DEventProcessor_TrackMLextract() : t_out(nullptr) {}
DEventProcessor_TrackMLextract::~DEventProcessor_TrackMLextract() {}

void DEventProcessor_TrackMLextract::Init(void)
{
    if (gFile) {
        gFile->cd();
    }
    t_out = new TTree("fdc_track_tree", "FDC Hits Matched to Tracks");
    t_out->Branch("EventID", &m_eventID, "EventID/I");
    t_out->Branch("TrackID", &m_trackID, "TrackID/I");
    t_out->Branch("PID", &m_pid, "PID/I");
    t_out->Branch("NHitsFDC", &m_nHitsFDC, "NHitsFDC/I");
    t_out->Branch("fdc_layer", &fdc_layer);
    t_out->Branch("fdc_wire", &fdc_wire);
    t_out->Branch("fdc_time", &fdc_time);
    t_out->Branch("fdc_charge", &fdc_charge);
    t_out->Branch("fdc_x", &fdc_x);
    t_out->Branch("fdc_y", &fdc_y);
    t_out->Branch("fdc_z", &fdc_z);
    t_out->Branch("fdc_truth_track", &fdc_truth_track);
    t_out->Branch("MCTrackID", &m_mcTrackID, "MCTrackID/I");
    t_out->Branch("TruthPurity", &m_truthPurity, "TruthPurity/F");
    t_out->Branch("measuredFDCHitsOnTrack", &m_measuredFDCHits, "measuredFDCHitsOnTrack/i");
    t_out->Branch("potentialFDCHitsOnTrack", &m_potentialFDCHits, "potentialFDCHitsOnTrack/i");
    t_out->Branch("FDCPlanesBitmask", &m_FDCPlanesBitmask, "FDCPlanesBitmask/i");
    t_out->Branch("trackE", &m_trackE, "trackE/F");
    t_out->Branch("trackP", &m_trackP, "trackP/F");
    t_out->Branch("trackTheta", &m_trackTheta, "trackTheta/F");
    t_out->Branch("trackPhi", &m_trackPhi, "trackPhi/F");
    t_out->Branch("match_px", &m_match_px, "match_px/F");
    t_out->Branch("match_py", &m_match_py, "match_py/F");
    t_out->Branch("match_pz", &m_match_pz, "match_pz/F");
}

void DEventProcessor_TrackMLextract::BeginRun(const std::shared_ptr<const JEvent>& event)
{
}

void DEventProcessor_TrackMLextract::Process(const std::shared_ptr<const JEvent>& event)
{
    vector<const DMCThrown*> throwns;
    event->Get(throwns);

    // DMCThrown is available only for primary generated particles.  It is
    // used below only when a truth track can be verified as such a primary,
    // so that decay daughters are never incorrectly assigned primary truth.
    map<int, const DMCThrown*> thrownByID;
    for (const auto* thrown : throwns) {
        thrownByID[thrown->myid] = thrown;
    }

    vector<const DTrackTimeBased*> tbts;
    event->Get(tbts);
    uint64_t eventnumber = event->GetEventNumber();
    uint32_t trackid = 0;

    for(auto tbt : tbts) {
        ClearVectors();
        std::map<int, unsigned int> truthTrackCounts;
        std::map<int, std::map<int, unsigned int>> truthPIDCounts;
        m_eventID = eventnumber;
        m_trackID = -1;
        const int recoPID = PDGtype(tbt->PID());
        m_pid = recoPID;

        int charge = tbt->charge();
        if (charge == 0) continue; // skip neutral tracks

        m_trackTheta = tbt->momentum().Theta()*180.0/TMath::Pi();
        m_trackPhi   = tbt->momentum().Phi()*180.0/TMath::Pi();
        m_trackP     = tbt->momentum().Mag();
        m_trackE     = tbt->energy();
        m_mcTrackID = -1;
        m_truthPurity = 0.0;
        m_match_px = -999.0;
        m_match_py = -999.0;
        m_match_pz = -999.0;

        // Diagnostics: what does the fit itself report about FDC association?
        m_measuredFDCHits  = tbt->measured_fdc_hits_on_track;
        m_potentialFDCHits = tbt->potential_fdc_hits_on_track;
        m_FDCPlanesBitmask = tbt->dFDCPlanes;

        vector<const DFDCPseudo*> fdchits;
        tbt->Get(fdchits);
        m_nHitsFDC = fdchits.size();
        for(auto hit : fdchits) {
            fdc_layer.push_back(hit->wire->layer);
            fdc_wire.push_back(hit->wire->wire);
            fdc_time.push_back(hit->time);
            fdc_charge.push_back(hit->dE);
            fdc_x.push_back(hit->xy.X());
            fdc_y.push_back(hit->xy.Y());
            fdc_z.push_back(hit->wire->origin.Z());

            // FDC:MATCH_TRUTH_HITS=1 attaches DMCTrackHit objects to the
            // pseudopoint. DFDCPseudo::itrack is not this truth association.
            vector<const DMCTrackHit*> mcTruthHits;
            hit->Get(mcTruthHits);
            if (mcTruthHits.empty()) {
                fdc_truth_track.push_back(-1);
                continue;
            }

            const auto* mcTruthHit = mcTruthHits.front();
            fdc_truth_track.push_back(mcTruthHit->track);
            truthTrackCounts[mcTruthHit->track]++;
            const int mcTruthPID =
                PDGtype(static_cast<Particle_t>(mcTruthHit->ptype));
            truthPIDCounts[mcTruthHit->track][mcTruthPID]++;
        }

        if (truthTrackCounts.empty()) continue;

        auto best = truthTrackCounts.begin();
        for (auto it = truthTrackCounts.begin(); it != truthTrackCounts.end(); ++it) {
            if (it->second > best->second) best = it;
        }

        // The dominant DMCTrackHit gives the GEANT track identity shared with
        // the DIRC truth-bar hits. Determine its truth PID independently.
        const int matchedMCTrackID = best->first;
        const auto& pidCounts = truthPIDCounts[matchedMCTrackID];
        auto bestPID = pidCounts.begin();
        for (auto it = pidCounts.begin(); it != pidCounts.end(); ++it) {
            if (it->second > bestPID->second) bestPID = it;
        }
        const int truthPID = bestPID->first;

        // The event-level list contains several mass hypotheses for each
        // physical trajectory. Keep only the hypothesis matching the FDC
        // truth PID, so one row is written per physical MC track.
        // cout << "FDC candidate:"
        // << " EventID=" << eventnumber
        // << " recoPID=" << recoPID
        // << " truthPID=" << truthPID
        // << " MCTrackID=" << matchedMCTrackID
        // << " FDC hits=" << fdchits.size()
        // << endl;
        if (recoPID != truthPID) continue;

        m_trackID = trackid++;
        m_pid = truthPID;
        m_mcTrackID = matchedMCTrackID;
        m_truthPurity = static_cast<float>(best->second) /
                        static_cast<float>(fdchits.size());

        // Decay daughters need a trajectory-truth lookup for their true
        // momentum. Fill it only when this GEANT track is verifiably a
        // matching primary DMCThrown particle.
        auto matchedThrownIt = thrownByID.find(matchedMCTrackID);
        if (matchedThrownIt != thrownByID.end() &&
            matchedThrownIt->second->pdgtype == truthPID) {
            const auto* matchedThrown = matchedThrownIt->second;
            m_match_px = matchedThrown->momentum().X();
            m_match_py = matchedThrown->momentum().Y();
            m_match_pz = matchedThrown->momentum().Z();
        }

        if (t_out) t_out->Fill();
    }
}

// void DEventProcessor_TrackMLextract::Process(const std::shared_ptr<const JEvent>& event)
// {
//     vector<const DTrackTimeBased*> tbts;
//     event->Get(tbts);
//     uint64_t eventnumber = event->GetEventNumber();
//     uint32_t trackid = 0;
//     int charge = 0;
//     for(auto tbt : tbts) {
//         ClearVectors();
//         m_eventID = eventnumber;
//         m_trackID = trackid++;
//         m_pid = PDGtype(tbt->PID());   // convert GlueX Particle_t -> real PDG code
//         charge = tbt->charge();
        
//         if (charge == 0) continue; // skip neutral tracks

//         vector<const DFDCPseudo*> fdchits;
//         tbt->Get(fdchits);
//         m_nHitsFDC = fdchits.size();
//         for(auto hit : fdchits) {
//             fdc_layer.push_back(hit->wire->layer);
//             fdc_wire.push_back(hit->wire->wire);
//             fdc_time.push_back(hit->time);
//             fdc_charge.push_back(hit->dE);
//             fdc_x.push_back(hit->xy.X());
//             fdc_y.push_back(hit->xy.Y());
//             fdc_z.push_back(hit->wire->origin.Z());
//         }
//         if (t_out) {
//             t_out->Fill();
//         }
//     }
// }

void DEventProcessor_TrackMLextract::ClearVectors()
{
    fdc_layer.clear();
    fdc_wire.clear();
    fdc_time.clear();
    fdc_charge.clear();
    fdc_x.clear();
    fdc_y.clear();
    fdc_z.clear();
    fdc_truth_track.clear();
}

void DEventProcessor_TrackMLextract::EndRun(void)
{
}

void DEventProcessor_TrackMLextract::Finish(void)
{
}
