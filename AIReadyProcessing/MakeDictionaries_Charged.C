#define glx__sim
#include "DrcEvent.h"
#include "glxtools.C"
#include <cmath>
#include <map>
#include <limits>

struct DircHitStruct {
    int PDG, MCTrackID;
    int BarID;
    Float_t invMass, P, Theta, Phi, X, Y, Z;
    Double_t LikelihoodElectron, LikelihoodPion, LikelihoodKaon, LikelihoodProton;
    vector<int> pmtID, pixelID, channel;
    vector<float> pos_x, pos_y, pos_z, leadTime;
    float px, py, pz;
};

struct FdcTrackStruct {
    int TrackID, PID, NHitsFDC, MCTrackID;
    float TruthPurity;
    vector<int> layer, wire;
    vector<float> time, charge, x, y, z;
    float px,py,pz;
};

struct FCALTrackStruct {
    int ShowerNumber, MCTrackID, PID;
    float showerX, showerY, showerZ, showerR, showerPhi, showerTheta;
    float showerE, showerT, E1E9, E9E25;
    int nMatched, nMerged;
    vector<float> hit_x, hit_y, hit_E, hit_t;
    vector<int> hit_row, hit_column;
    int NHitsFCAL;
    float px, py, pz;
};

void WriteDircJson(DircHitStruct& d, std::ofstream& out)
{
    out << "{"
        << Form("\"PID\":%d,", d.PDG)
        << Form("\"MCTrackID\":%d,", d.MCTrackID)
        << Form("\"BarID\":%d,", d.BarID)
        << Form("\"invMass\":%f,", d.invMass)
        << Form("\"P\":%f,", d.P)
        << Form("\"Theta\":%f,", d.Theta)
        << Form("\"Phi\":%f,", d.Phi)
        << Form("\"X\":%f,\"Y\":%f,\"Z\":%f,", d.X, d.Y, d.Z)
        << Form("\"LikelihoodElectron\":%f,", d.LikelihoodElectron)
        << Form("\"LikelihoodPion\":%f,", d.LikelihoodPion)
        << Form("\"LikelihoodKaon\":%f,", d.LikelihoodKaon)
        << Form("\"LikelihoodProton\":%f,", d.LikelihoodProton)
        << "\"pmtID\":[";
    for (size_t i=0;i<d.pmtID.size();i++) out << d.pmtID[i] << (i+1<d.pmtID.size()?",":"");
    out << "],\"pixelID\":[";
    for (size_t i=0;i<d.pixelID.size();i++) out << d.pixelID[i] << (i+1<d.pixelID.size()?",":"");
    out << "],\"channel\":[";
    for (size_t i=0;i<d.channel.size();i++) out << d.channel[i] << (i+1<d.channel.size()?",":"");
    out << "],\"pos_x\":[";
    for (size_t i=0;i<d.pos_x.size();i++) out << d.pos_x[i] << (i+1<d.pos_x.size()?",":"");
    out << "],\"pos_y\":[";
    for (size_t i=0;i<d.pos_y.size();i++) out << d.pos_y[i] << (i+1<d.pos_y.size()?",":"");
    out << "],\"pos_z\":[";
    for (size_t i=0;i<d.pos_z.size();i++) out << d.pos_z[i] << (i+1<d.pos_z.size()?",":"");
    out << "],\"leadTime\":[";
    for (size_t i=0;i<d.leadTime.size();i++) out << d.leadTime[i] << (i+1<d.leadTime.size()?",":"");
    out << "]}";
}

void WriteFdcJson(FdcTrackStruct& f, std::ofstream& out)
{
    out << "{"
        << Form("\"TrackID\":%d,", f.TrackID)
        << Form("\"PID\":%d,", f.PID)
        << Form("\"MCTrackID\":%d,", f.MCTrackID)
        << Form("\"TruthPurity\":%f,", f.TruthPurity)
        << Form("\"NHitsFDC\":%d,", f.NHitsFDC)
        << "\"layer\":[";
    for (size_t i=0;i<f.layer.size();i++) out << f.layer[i] << (i+1<f.layer.size()?",":"");
    out << "],\"wire\":[";
    for (size_t i=0;i<f.wire.size();i++) out << f.wire[i] << (i+1<f.wire.size()?",":"");
    out << "],\"time\":[";
    for (size_t i=0;i<f.time.size();i++) out << f.time[i] << (i+1<f.time.size()?",":"");
    out << "],\"charge\":[";
    for (size_t i=0;i<f.charge.size();i++) out << f.charge[i] << (i+1<f.charge.size()?",":"");
    out << "],\"x\":[";
    for (size_t i=0;i<f.x.size();i++) out << f.x[i] << (i+1<f.x.size()?",":"");
    out << "],\"y\":[";
    for (size_t i=0;i<f.y.size();i++) out << f.y[i] << (i+1<f.y.size()?",":"");
    out << "],\"z\":[";
    for (size_t i=0;i<f.z.size();i++) out << f.z[i] << (i+1<f.z.size()?",":"");
    out << "]}";
}

void WriteFcalJson(FCALTrackStruct& f, std::ofstream& out)
{
    out << "{"
        << Form("\"ShowerNumber\":%d,", f.ShowerNumber)
        << Form("\"MCTrackID\":%d,", f.MCTrackID)
        << Form("\"PID\":%d,", f.PID)
        << Form("\"showerX\":%f,", f.showerX)
        << Form("\"showerY\":%f,", f.showerY)
        << Form("\"showerZ\":%f,", f.showerZ)
        << Form("\"showerR\":%f,", f.showerR)
        << Form("\"showerPhi\":%f,", f.showerPhi)
        << Form("\"showerTheta\":%f,", f.showerTheta)
        << Form("\"showerE\":%f,", f.showerE)
        << Form("\"showerT\":%f,", f.showerT)
        << Form("\"E1E9\":%f,", f.E1E9)
        << Form("\"E9E25\":%f,", f.E9E25)
        << Form("\"nMatched\":%d,", f.nMatched)
        << Form("\"nMerged\":%d,", f.nMerged)
        << Form("\"NHitsFCAL\":%d,", f.NHitsFCAL)
        << "\"hit_x\":[";
    for (size_t i=0; i<f.hit_x.size(); i++) out << f.hit_x[i] << (i+1<f.hit_x.size()?",":"");
    out << "],\"hit_y\":[";
    for (size_t i=0; i<f.hit_y.size(); i++) out << f.hit_y[i] << (i+1<f.hit_y.size()?",":"");
    out << "],\"hit_E\":[";
    for (size_t i=0; i<f.hit_E.size(); i++) out << f.hit_E[i] << (i+1<f.hit_E.size()?",":"");
    out << "],\"hit_t\":[";
    for (size_t i=0; i<f.hit_t.size(); i++) out << f.hit_t[i] << (i+1<f.hit_t.size()?",":"");
    out << "],\"hit_row\":[";
    for (size_t i=0; i<f.hit_row.size(); i++) out << f.hit_row[i] << (i+1<f.hit_row.size()?",":"");
    out << "],\"hit_column\":[";
    for (size_t i=0; i<f.hit_column.size(); i++) out << f.hit_column[i] << (i+1<f.hit_column.size()?",":"");
    out << "]}";
}

void MakeDictionaries_Charged(TString inFileName, TString outFileName = "Phi.json",
                              bool printTrackMatching = true)
{


    if(!glx_initc(inFileName,1,"data/drawHP")) return;
    const int nEvents = glx_ch->GetEntries();
    cout << "Total number of Entries found : " << nEvents << endl;
    int bad_counter = 0;

    // --- Bind the new EventID branch on the DIRC tree/chain ---
    ULong64_t dircEventID = 0;
    glx_ch->SetBranchAddress("EventID", &dircEventID);

    // --- Grab fdc_track_tree from the SAME already-open file ---
    TFile *f = glx_ch->GetCurrentFile();
    TTree *fdcTree = (TTree*)f->Get("fdc_track_tree");
    if (!fdcTree){
        cout << "WARNING: fdc_track_tree not found in " << inFileName << endl;
        return;
    }


    int fEventID, fTrackID, fPID, fNHitsFDC, fMCTrackID;
    float fTruthPurity;
    vector<int> *fLayer=0, *fWire=0;
    vector<float> *fTime=0, *fCharge=0, *fX=0, *fY=0, *fZ=0;
    float fdc_match_px, fdc_match_py, fdc_match_pz;
    fdcTree->SetBranchAddress("EventID", &fEventID);
    fdcTree->SetBranchAddress("TrackID", &fTrackID);
    fdcTree->SetBranchAddress("PID", &fPID);
    fdcTree->SetBranchAddress("NHitsFDC", &fNHitsFDC);
    fdcTree->SetBranchAddress("MCTrackID", &fMCTrackID);
    fdcTree->SetBranchAddress("TruthPurity", &fTruthPurity);
    fdcTree->SetBranchAddress("fdc_layer", &fLayer);
    fdcTree->SetBranchAddress("fdc_wire", &fWire);
    fdcTree->SetBranchAddress("fdc_time", &fTime);
    fdcTree->SetBranchAddress("fdc_charge", &fCharge);
    fdcTree->SetBranchAddress("fdc_x", &fX);
    fdcTree->SetBranchAddress("fdc_y", &fY);
    fdcTree->SetBranchAddress("fdc_z", &fZ);
    fdcTree->SetBranchAddress("match_px", &fdc_match_px);
    fdcTree->SetBranchAddress("match_py", &fdc_match_py);
    fdcTree->SetBranchAddress("match_pz", &fdc_match_pz);

    std::map<int, vector<Long64_t>> eventToFdcRows;
    Long64_t nFdc = fdcTree->GetEntries();
    for (Long64_t i=0; i<nFdc; i++){
        fdcTree->GetEntry(i);
        eventToFdcRows[fEventID].push_back(i);
    }

    // --- fcal_track_tree ---
    TTree *fcalTree = (TTree*)f->Get("fcal_track_tree");
    if (!fcalTree){ cout << "WARNING: fcal_track_tree not found" << endl; return; }
 
    int eventID, showerNumber, nMatched, nMerged, matchedMCTrackID, thrownPID, nHits;
    bool matchThrown;
    float x, y, z, r, phi, theta, energy, time, e1e9, e9e25;
    vector<int> *rows=0, *columns=0;
    vector<float> *xs=0, *ys=0, *energies=0, *times=0;
    int unmatchedCount = 0;
    float m_match_px, m_match_py, m_match_pz;

    fcalTree->SetBranchAddress("eventnumber", &eventID);
    fcalTree->SetBranchAddress("showernumber", &showerNumber);
    fcalTree->SetBranchAddress("recon_FCAL_x", &x);
    fcalTree->SetBranchAddress("recon_FCAL_y", &y);
    fcalTree->SetBranchAddress("recon_FCAL_z", &z);
    fcalTree->SetBranchAddress("recon_FCAL_r", &r);
    fcalTree->SetBranchAddress("recon_FCAL_phi", &phi);
    fcalTree->SetBranchAddress("recon_FCAL_theta", &theta);
    fcalTree->SetBranchAddress("recon_FCAL_E", &energy);
    fcalTree->SetBranchAddress("recon_FCAL_t", &time);
    fcalTree->SetBranchAddress("recon_FCAL_E1E9", &e1e9);
    fcalTree->SetBranchAddress("recon_FCAL_E9E25", &e9e25);
    fcalTree->SetBranchAddress("recon_FCAL_nHits", &nHits);
    fcalTree->SetBranchAddress("recon_FCAL_match_thrown", &matchThrown);
    fcalTree->SetBranchAddress("matched_MCTrackID", &matchedMCTrackID);
    fcalTree->SetBranchAddress("n_matched", &nMatched);
    fcalTree->SetBranchAddress("m_n_merged", &nMerged);
    fcalTree->SetBranchAddress("thrown_ptype", &thrownPID);
    fcalTree->SetBranchAddress("match_px", &m_match_px);
    fcalTree->SetBranchAddress("match_py", &m_match_py);
    fcalTree->SetBranchAddress("match_pz", &m_match_pz);
    fcalTree->SetBranchAddress("fcal_row", &rows);
    fcalTree->SetBranchAddress("fcal_column", &columns);
    fcalTree->SetBranchAddress("fcal_x", &xs);
    fcalTree->SetBranchAddress("fcal_y", &ys);
    fcalTree->SetBranchAddress("fcal_E", &energies);
    fcalTree->SetBranchAddress("fcal_t", &times);

    map<int, vector<Long64_t>> eventToFcalRows;
    for (Long64_t i=0; i<fcalTree->GetEntries(); i++) {
        fcalTree->GetEntry(i);
        eventToFcalRows[eventID].push_back(i);
    }

    ofstream outFile;
    outFile.open(outFileName.Data());

    int nDircTracks = 0;
    int nDircTracksNoFdc = 0;
    int nFdcDircOnly = 0;
    int nFdcDircFcal = 0;
    int nJsonTracksWritten = 0;
    int nTwoTrackDircEvents = 0;
    int nTwoTrackEventsBothFdc = 0;
    int nTwoTrackEventsBothFdcFcal = 0;

    int nEventsSkippedTooManyDircTracks = 0;
    int nEventsSkippedSameOpticalBox = 0;
    int nEventsNoFdcRows = 0;
    int nEventsNoFcalRows = 0;

    int nFdcRowsNoMatchingMCID = 0;
    int nFdcRowsPIDMismatch = 0;
    int nFcalRowsNoMatchingMCID = 0;
    int nFcalRowsPIDMismatch = 0;

    int nJsonEventsWritten = 0;

    for (int ev = 0; ev < nEvents; ev++)
    {
        glx_ch->GetEntry(ev);              // this now also fills dircEventID
        int realEventID = (int)dircEventID; // the true JANA event number for this row

        //int charge = glx_event->GetCharge();

        const int ps = glx_events->GetEntriesFast();
        vector<int> physicalDircIndices;

        for (int p = 0; p < ps; p++) {
            glx_nextEventc(ev, p, 10);

            if (glx_event->GetPdg() == 0) {
                bad_counter++;
                continue;
            }

            physicalDircIndices.push_back(p);
        }

        // Temporary diagnostic for determining why one member of a K+K-
        // pair may be absent from the JSON output.
        bool debugThisEvent = false;
        if (printTrackMatching) {
            for (int p : physicalDircIndices) {
                glx_nextEventc(ev, p, 10);
                if (abs(glx_event->GetPdg()) == 321) {
                    // debugThisEvent = true; 
                    // Remove above comment for normal operation. Only enable for debugging.
                    break;
                }
            }
        }

        if (debugThisEvent) {
            cout << "\nDIRC diagnostic: EventID=" << realEventID
                 << ", labelled DIRC tracks=" << physicalDircIndices.size()
                 << endl;
            for (int p : physicalDircIndices) {
                glx_nextEventc(ev, p, 10);
                cout << "  DIRC entry=" << p
                     << " PID=" << glx_event->GetPdg()
                     << " MCTrackID=" << glx_event->GetTrackID()
                     << " Y=" << glx_event->GetPosition().Y()
                     << " DIRC hits=" << glx_event->GetHitSize()
                     << endl;
            }
        }

        // Gather FDC rows before DIRC multiplicity handling so that duplicate
        // reaction hypotheses for one MC track can be resolved below. Do not
        // increment the no-FDC counter here: preserve its existing meaning by
        // counting only events that pass the DIRC optical-box selection.
        vector<FdcTrackStruct> allFdcForEvent;
        auto it = eventToFdcRows.find(realEventID);
        if (it != eventToFdcRows.end()) {
            for (Long64_t row : it->second) {
                fdcTree->GetEntry(row);
                FdcTrackStruct fEntry;
                fEntry.TrackID = fTrackID;
                fEntry.PID = fPID;
                fEntry.MCTrackID = fMCTrackID;
                fEntry.TruthPurity = fTruthPurity;
                fEntry.NHitsFDC = fNHitsFDC;
                fEntry.px = fdc_match_px;
                fEntry.py = fdc_match_py;
                fEntry.pz = fdc_match_pz;
                fEntry.layer = *fLayer;
                fEntry.wire = *fWire;
                fEntry.time = *fTime;
                fEntry.charge = *fCharge;
                fEntry.x = *fX;
                fEntry.y = *fY;
                fEntry.z = *fZ;
                allFdcForEvent.push_back(fEntry);
            }
        }

        // DIRC can contain multiple reaction hypotheses for the same physical
        // GEANT track. Keep one entry per MCTrackID before applying the
        // two-track and optical-box requirements. Prefer the hypothesis whose
        // PID matches an FDC row for that same MC track.
        auto HasMatchingFdc = [&](int mcid, int pid)
        {
            for (const auto& fdcEntry : allFdcForEvent) {
                if (fdcEntry.MCTrackID == mcid && fdcEntry.PID == pid)
                    return true;
            }
            return false;
        };

        vector<int> uniqueDircIndices;
        map<int, size_t> uniqueDircPositionByMCTrackID;
        vector<bool> uniqueDircHasFdcMatch;

        for (int p : physicalDircIndices) {
            glx_nextEventc(ev, p, 10);

            const int mcid = glx_event->GetTrackID();
            const int pid = glx_event->GetPdg();
            const bool hasFdcMatch = HasMatchingFdc(mcid, pid);
            auto selected = uniqueDircPositionByMCTrackID.find(mcid);

            if (selected == uniqueDircPositionByMCTrackID.end()) {
                uniqueDircPositionByMCTrackID[mcid] = uniqueDircIndices.size();
                uniqueDircIndices.push_back(p);
                uniqueDircHasFdcMatch.push_back(hasFdcMatch);
            }
            else if (hasFdcMatch && !uniqueDircHasFdcMatch[selected->second]) {
                uniqueDircIndices[selected->second] = p;
                uniqueDircHasFdcMatch[selected->second] = true;
            }
        }

        physicalDircIndices = uniqueDircIndices;

        if (debugThisEvent) {
            cout << "  After MCTrackID deduplication: "
                 << physicalDircIndices.size() << " physical DIRC tracks" << endl;
            for (int p : physicalDircIndices) {
                glx_nextEventc(ev, p, 10);
                cout << "    selected PID=" << glx_event->GetPdg()
                     << " MCTrackID=" << glx_event->GetTrackID()
                     << " FDC PID match="
                     << (HasMatchingFdc(glx_event->GetTrackID(),
                                        glx_event->GetPdg()) ? "yes" : "no")
                     << endl;
            }
        }

        if (physicalDircIndices.size() > 2)
        {
            if (debugThisEvent)
                cout << "  Rejected: more than two labelled DIRC tracks." << endl;
            nEventsSkippedTooManyDircTracks++;
            continue;
        }

        // DIRC events with multiple tracks - must hit separate optical boxes
        // Optical boxes are identified via modulus - offset indicies
        if (physicalDircIndices.size() == 2) {
            glx_nextEventc(ev, physicalDircIndices[0], 10);
            float firstTrackY = glx_event->GetPosition().Y();

            glx_nextEventc(ev, physicalDircIndices[1], 10);
            float secondTrackY = glx_event->GetPosition().Y();

            if ((firstTrackY < 0 && secondTrackY < 0) ||
                (firstTrackY > 0 && secondTrackY > 0)) {
                if (debugThisEvent)
                    cout << "  Rejected: both labelled DIRC tracks are in the same optical box."
                         << endl;
                nEventsSkippedSameOpticalBox++;
                continue;
            }
        }

        vector<DircHitStruct> dircEntries;
        vector<int> dircPDGs;
        vector<int> dircMCTrackIDs;

        for (int p : physicalDircIndices)
        {
            glx_nextEventc(ev,p,10);

            DircHitStruct d;
            // Get the momentum components from the matched glx_event object
            // Will associate these later as well
            d.px = glx_event->GetMomentum_Truth().X();
            d.py = glx_event->GetMomentum_Truth().Y();
            d.pz = glx_event->GetMomentum_Truth().Z();

            d.PDG = glx_event->GetPdg();
            d.MCTrackID = glx_event->GetTrackID();
            int nh = glx_event->GetHitSize();
            d.BarID = glx_event->GetId();
            d.P = glx_event->GetMomentum().Mag();
            d.Theta = glx_event->GetMomentum().Theta()*180/TMath::Pi();
            d.Phi = glx_event->GetMomentum().Phi()*180/TMath::Pi();
            TVector3 hpos = glx_event->GetPosition();
            d.X = hpos.X(); d.Y = hpos.Y(); d.Z = hpos.Z();
            d.invMass = glx_event->GetInvMass();
            d.LikelihoodElectron = glx_event->GetLikelihoodElectron();
            d.LikelihoodPion = glx_event->GetLikelihoodPion();
            d.LikelihoodKaon = glx_event->GetLikelihoodKaon();
            d.LikelihoodProton = glx_event->GetLikelihoodProton();

            if (std::isnan(d.LikelihoodElectron)) d.LikelihoodElectron = -99999;
            if (std::isnan(d.LikelihoodPion)) d.LikelihoodPion = -99999;
            if (std::isnan(d.LikelihoodKaon)) d.LikelihoodKaon = -99999;
            if (std::isnan(d.LikelihoodProton)) d.LikelihoodProton = -99999;

            // In two-track events, DIRC optical boxes are identified by the
            // offset PMT number: pmtID // 108 gives box 0 or 1. The Python
            // post-processing assigns box 0 to the Y<0 track and box 1 to
            // the Y>0 track, so apply the same rule here.
            int expectedOpticalBox = -1;
            if (physicalDircIndices.size() == 2) {
                if (d.Y < 0) expectedOpticalBox = 0;
                else if (d.Y > 0) expectedOpticalBox = 1;
            }
            for (int h = 0; h < nh; h++)
            {
                DrcHit hit = glx_event->GetHit(h);
                if (expectedOpticalBox >= 0 &&
                    hit.GetPmtId() / 108 != expectedOpticalBox)
                    continue;
                d.pmtID.push_back(hit.GetPmtId());
                d.pixelID.push_back(hit.GetPixelId());
                d.channel.push_back(hit.GetChannel());
                d.pos_x.push_back(hit.GetPosition().X());
                d.pos_y.push_back(hit.GetPosition().Y());
                d.pos_z.push_back(hit.GetPosition().Z());
                d.leadTime.push_back(hit.GetLeadTime());
            }
            dircEntries.push_back(d);
            dircPDGs.push_back(d.PDG);
            dircMCTrackIDs.push_back(d.MCTrackID);
        }

        // --- FDC rows were gathered above for DIRC hypothesis resolution ---
        if (allFdcForEvent.empty()) nEventsNoFdcRows++;

        // --- Keep only FDC tracks whose MCTrackID appears in the DIRC ---
        // Lets also check thrown momentum matches within reasonable tolerance
        vector<FdcTrackStruct> fdcEntries;

        for (auto& fEntry : allFdcForEvent) {
            bool hasSameMCTrackID = false;
            bool matchedToDIRC = false;

            for (const auto& dircEntry : dircEntries) {
                if (fEntry.MCTrackID < 0)
                    continue;

                if (fEntry.MCTrackID != dircEntry.MCTrackID)
                    continue;

                hasSameMCTrackID = true;

                if (fEntry.PID != dircEntry.PDG) {
                    nFdcRowsPIDMismatch++;
                    continue;
                }

                matchedToDIRC = true;
                break;
            }

            if (matchedToDIRC) {
                fdcEntries.push_back(fEntry);
            }
            else if (!hasSameMCTrackID) {
                nFdcRowsNoMatchingMCID++;
            }
        }

        // --- FCAL: gather all tracks for this event
        // FCAL might not exist for a charged track 
        vector<FCALTrackStruct> allFcalForEvent;
        auto itFcal = eventToFcalRows.find(realEventID);
        if (itFcal != eventToFcalRows.end()){
            for (Long64_t row : itFcal->second){
                fcalTree->GetEntry(row);
                
                if (!matchThrown || matchedMCTrackID < 0) // check FCAL thrown match
                    continue;

                FCALTrackStruct f;
                f.ShowerNumber = showerNumber;
                f.MCTrackID = matchedMCTrackID;
                f.PID = thrownPID;
                f.showerX = x; f.showerY = y; f.showerZ = z;
                f.showerR = r; f.showerPhi = phi; f.showerTheta = theta;
                f.showerE = energy; f.showerT = time;
                f.E1E9 = e1e9; f.E9E25 = e9e25;
                f.nMatched = nMatched; f.nMerged = nMerged;
                f.px = m_match_px; f.py = m_match_py; f.pz = m_match_pz;
                if (rows) for (size_t i=0; i<rows->size(); i++) f.hit_row.push_back((*rows)[i]);
                if (columns) for (size_t i=0; i<columns->size(); i++) f.hit_column.push_back((*columns)[i]);
                if (xs) for (size_t i=0; i<xs->size(); i++) f.hit_x.push_back((*xs)[i]);
                if (ys) for (size_t i=0; i<ys->size(); i++) f.hit_y.push_back((*ys)[i]);
                if (energies) for (size_t i=0; i<energies->size(); i++) f.hit_E.push_back((*energies)[i]);
                if (times) for (size_t i=0; i<times->size(); i++) f.hit_t.push_back((*times)[i]);
                f.NHitsFCAL = f.hit_x.size();
                allFcalForEvent.push_back(f);
            }
        } else {
            nEventsNoFcalRows++;
        }

        if (debugThisEvent) {
            cout << "  FDC rows in event:";
            if (allFdcForEvent.empty()) cout << " none";
            cout << endl;
            for (const auto& fdcEntry : allFdcForEvent) {
                cout << "    PID=" << fdcEntry.PID
                     << " MCTrackID=" << fdcEntry.MCTrackID
                     << " NHits=" << fdcEntry.NHitsFDC
                     << " purity=" << fdcEntry.TruthPurity
                     << endl;
            }

            cout << "  FCAL rows in event:";
            if (allFcalForEvent.empty()) cout << " none";
            cout << endl;
            for (const auto& fcalEntry : allFcalForEvent) {
                cout << "    PID=" << fcalEntry.PID
                     << " MCTrackID=" << fcalEntry.MCTrackID
                     << " NHits=" << fcalEntry.NHitsFCAL
                     << endl;
            }
        }

        vector<FCALTrackStruct> fcalEntries;

        for (const auto& fcalEntry : allFcalForEvent) {
            bool hasSameMCTrackID = false;
            bool matchedToDIRC = false;

            if (fcalEntry.MCTrackID < 0)
                continue;

            for (const auto& dircEntry : dircEntries) {
                if (fcalEntry.MCTrackID < 0)
                    continue;

                if (fcalEntry.MCTrackID != dircEntry.MCTrackID)
                    continue;

                hasSameMCTrackID = true;

                if (fcalEntry.PID != dircEntry.PDG) {
                    nFcalRowsPIDMismatch++;
                    continue;
                }

                matchedToDIRC = true;
                break;
            }

            if (matchedToDIRC) {
                fcalEntries.push_back(fcalEntry);
            }
            else if (!hasSameMCTrackID) {
                nFcalRowsNoMatchingMCID++;
            }
        }

        if (debugThisEvent) {
            for (const auto& dircEntry : dircEntries) {
                bool hasFdcMatch = false;
                bool hasFcalMatch = false;

                for (const auto& fdcEntry : fdcEntries) {
                    if (fdcEntry.MCTrackID == dircEntry.MCTrackID &&
                        fdcEntry.PID == dircEntry.PDG) {
                        hasFdcMatch = true;
                        break;
                    }
                }
                for (const auto& fcalEntry : fcalEntries) {
                    if (fcalEntry.MCTrackID == dircEntry.MCTrackID &&
                        fcalEntry.PID == dircEntry.PDG) {
                        hasFcalMatch = true;
                        break;
                    }
                }

                cout << "  Match status: PID=" << dircEntry.PDG
                     << " MCTrackID=" << dircEntry.MCTrackID
                     << " FDC=" << (hasFdcMatch ? "yes" : "no")
                     << " FCAL=" << (hasFcalMatch ? "yes" : "no")
                     << endl;
            }
        }

        vector<int> particleMCTrackIDs;
        for (const auto& dircEntry : dircEntries) {
            if (std::find(particleMCTrackIDs.begin(), particleMCTrackIDs.end(), dircEntry.MCTrackID) == particleMCTrackIDs.end()) {
                particleMCTrackIDs.push_back(dircEntry.MCTrackID);
            }
        }

        // Count retained two-track events using unique MC track IDs. This
        // avoids treating alternate reaction hypotheses for one trajectory
        // as two separate charged tracks.
        if (particleMCTrackIDs.size() == 2) {
            nTwoTrackDircEvents++;

            bool bothHaveFdc = true;
            bool bothHaveFcal = true;
            for (const int mcid : particleMCTrackIDs) {
                bool hasFdc = false;
                bool hasFcal = false;

                for (const auto& fdcEntry : fdcEntries) {
                    if (fdcEntry.MCTrackID == mcid) {
                        hasFdc = true;
                        break;
                    }
                }
                for (const auto& fcalEntry : fcalEntries) {
                    if (fcalEntry.MCTrackID == mcid) {
                        hasFcal = true;
                        break;
                    }
                }

                bothHaveFdc = bothHaveFdc && hasFdc;
                bothHaveFcal = bothHaveFcal && hasFcal;
            }

            if (bothHaveFdc) nTwoTrackEventsBothFdc++;
            if (bothHaveFdc && bothHaveFcal) nTwoTrackEventsBothFdcFcal++;
        }

        // First decide which DIRC tracks survive the existing FDC requirement.
        // This preserves the present single-track behavior.
        vector<int> retainedMCTrackIDs;

        for (int mcid : particleMCTrackIDs) {
            nDircTracks++;

            float px = 0.0;
            float py = 0.0;
            float pz = 0.0;
            int dircPID = 0;

            for (const auto& d : dircEntries) {
                if (d.MCTrackID == mcid) {
                    px = d.px;
                    py = d.py;
                    pz = d.pz;
                    dircPID = d.PDG;
                    break;
                }
            }

            bool hasFdc = false;
            bool hasFcal = false;

            for (const auto& fdcEntry : fdcEntries) {
                if (fdcEntry.MCTrackID == mcid) {
                    hasFdc = true;
                    break;
                }
            }

            for (const auto& fcalEntry : fcalEntries) {
                if (fcalEntry.MCTrackID == mcid) {
                    hasFcal = true;
                    break;
                }
            }

            if (!hasFdc && debugThisEvent) {
                cout << "Theta: "
                    << TMath::RadToDeg() * atan2(sqrt(px*px + py*py), pz)
                    << " deg, Phi: "
                    << TMath::RadToDeg() * atan2(py, px)
                    << " deg, P: "
                    << sqrt(px*px + py*py + pz*pz)
                    << " GeV/c, MCTrackID: " << mcid
                    << ", PID: " << dircPID
                    << " has DIRC but no FDC match. Skipping." << endl;

                nDircTracksNoFdc++;
                continue;
            }

            if (hasFcal)
                nFdcDircFcal++;
            else
                nFdcDircOnly++;

            nJsonTracksWritten++;
            retainedMCTrackIDs.push_back(mcid);
        }

        // Do not write an event with no retained charged tracks.
        if (retainedMCTrackIDs.empty())
            continue;

        if (debugThisEvent) {
            if (retainedMCTrackIDs.size() > 1){
            cout << "***********************************************************" << endl;
            cout << "Event " << realEventID << "  Retained " << retainedMCTrackIDs.size()
                 << " DIRC tracks for JSON output." << endl;
            cout << "***********************************************************" << endl;
            }
        }

        // Write one JSON object per event.
        // A single-track gun naturally produces Tracks with one element.
        nJsonEventsWritten++;

        outFile << "{";
        outFile << Form("\"EventID\":%d,", realEventID);
        outFile << "\"Tracks\":[";

        for (size_t trackIndex = 0;
            trackIndex < retainedMCTrackIDs.size();
            trackIndex++) {

            const int mcid = retainedMCTrackIDs[trackIndex];

            float px = 0.0;
            float py = 0.0;
            float pz = 0.0;
            int dircPID = 0;

            for (const auto& d : dircEntries) {
                if (d.MCTrackID == mcid) {
                    px = d.px;
                    py = d.py;
                    pz = d.pz;
                    dircPID = d.PDG;
                    break;
                }
            }

            outFile << "{";
            outFile << Form("\"MCTrackID\":%d,", mcid);
            outFile << Form("\"PID\":%d,", dircPID);
            outFile << Form("\"px\":%f,", px);
            outFile << Form("\"py\":%f,", py);
            outFile << Form("\"pz\":%f,", pz);

            outFile << "\"DIRC\":[";
            bool firstDirc = true;
            for (auto& dircEntry : dircEntries) {
                if (dircEntry.MCTrackID != mcid)
                    continue;

                if (!firstDirc)
                    outFile << ",";

                WriteDircJson(dircEntry, outFile);
                firstDirc = false;
            }
            outFile << "],";

            outFile << "\"FDC\":[";
            bool firstFdc = true;
            for (auto& fdcEntry : fdcEntries) {
                if (fdcEntry.MCTrackID != mcid)
                    continue;

                if (!firstFdc)
                    outFile << ",";

                WriteFdcJson(fdcEntry, outFile);
                firstFdc = false;
            }
            outFile << "],";

            outFile << "\"FCAL\":[";
            bool firstFcal = true;
            for (auto& fcalEntry : fcalEntries) {
                if (fcalEntry.MCTrackID != mcid)
                    continue;

                if (!firstFcal)
                    outFile << ",";

                WriteFcalJson(fcalEntry, outFile);
                firstFcal = false;
            }
            outFile << "]";

            outFile << "}";

            if (trackIndex + 1 < retainedMCTrackIDs.size())
                outFile << ",";
        }

        outFile << "]}" << endl;

    }
    
    outFile.close();
    cout << "\nCharged-track summary" << endl;
    cout << "DIRC tracks considered: " << nDircTracks << endl;
    cout << "Tracks lost: no matching FDC: " << nDircTracksNoFdc << endl;
    cout << "Written FDC + DIRC: " << nFdcDircOnly << endl;
    cout << "Written FDC + DIRC + FCAL: " << nFdcDircFcal << endl;
    cout << "Total JSON events written: " << nJsonEventsWritten << endl;
    cout << "Total JSON tracks written: " << nJsonTracksWritten << endl;

    cout << "\nTwo-track event retention" << endl;
    cout << "Events with two distinct DIRC tracks retained: " << nTwoTrackDircEvents << endl;
    cout << "Events retaining both tracks with FDC + DIRC: " << nTwoTrackEventsBothFdc << endl;
    cout << "Events retaining both tracks with FDC + DIRC + FCAL: " << nTwoTrackEventsBothFdcFcal << endl;

    cout << "\nEvent rejections" << endl;
    cout << "Unlabelled calibrated DIRC records skipped (PDG = 0): " << bad_counter << endl;
    cout << "Events skipped: more than two DIRC tracks: " << nEventsSkippedTooManyDircTracks << endl;
    cout << "Events skipped: two tracks in the same optical box: " << nEventsSkippedSameOpticalBox << endl;
    cout << "DIRC events with no FDC rows: " << nEventsNoFdcRows << endl;
    cout << "DIRC events with no FCAL rows: " << nEventsNoFcalRows << endl;

    cout << "\nCandidate-match failures" << endl;
    cout << "FDC rows with no matching DIRC MCTrackID: " << nFdcRowsNoMatchingMCID << endl;
    cout << "FDC rows with matching MCTrackID but wrong PID: " << nFdcRowsPIDMismatch << endl;
    cout << "FCAL rows with no matching DIRC MCTrackID: " << nFcalRowsNoMatchingMCID << endl;
    cout << "FCAL rows with matching MCTrackID but wrong PID: " << nFcalRowsPIDMismatch << endl;
}
