#define glx__sim
#include "glxtools.C"

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

void MakeDictionaries_Neutral(TString inFileName, TString outFileName = "Phi.json")
{
    if(!glx_initc(inFileName, 1, "data/drawHP")) return;

    TFile *file = glx_ch->GetCurrentFile();
    if (!file || file->IsZombie()) return;

    TTree *fcalTree = (TTree*)file->Get("fcal_track_tree");
    if (!fcalTree){ cout << "WARNING: fcal_track_tree not found" << endl; return; }

    int eventID, showerNumber, nMatched, nMerged, matchedMCTrackID, thrownPID, nHits;
    bool matchThrown;
    float x, y, z, r, phi, theta, energy, time, e1e9, e9e25;
    vector<int> *rows=0, *columns=0;
    vector<float> *xs=0, *ys=0, *energies=0, *times=0;
    int unmatchedCount = 0;
    int nJsonEventsWritten = 0;
    int nJsonTracksWritten = 0;
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

    ofstream outFile;
    outFile.open(outFileName.Data());

    map<int, vector<Long64_t>> eventToRows;
    for (Long64_t i=0; i<fcalTree->GetEntries(); i++) {
        fcalTree->GetEntry(i);
        eventToRows[eventID].push_back(i);
    }


    for (auto const& eventRows : eventToRows) {
        vector<FCALTrackStruct> neutralTracks;

        for (Long64_t row : eventRows.second) {
            fcalTree->GetEntry(row);

            if (!matchThrown || matchedMCTrackID < 0){
                unmatchedCount++;
                continue;
            }

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

            neutralTracks.push_back(f);
        }

        // Write one JSON object per event. Each matched FCAL shower is one
        // neutral-particle entry, so single- and multi-neutral events share
        // the same Tracks-list schema as the charged dictionary.
        if (neutralTracks.empty())
            continue;

        nJsonEventsWritten++;
        nJsonTracksWritten += neutralTracks.size();

        outFile << "{" << Form("\"EventID\":%d,", eventRows.first);
        outFile << "\"Tracks\":[";

        for (size_t trackIndex = 0; trackIndex < neutralTracks.size(); trackIndex++) {
            FCALTrackStruct& f = neutralTracks[trackIndex];

            outFile << "{";
            outFile << Form("\"MCTrackID\":%d,", f.MCTrackID);
            outFile << Form("\"PID\":%d,", f.PID);
            outFile << Form("\"px\":%f,", f.px);
            outFile << Form("\"py\":%f,", f.py);
            outFile << Form("\"pz\":%f,", f.pz);
            outFile << "\"FCAL\":[";
            WriteFcalJson(f, outFile);
            outFile << "]}";

            if (trackIndex + 1 < neutralTracks.size())
                outFile << ",";
        }

        outFile << "]}" << endl;
    }

    outFile.close();
    cout << "Unmatched showers: " << unmatchedCount << endl;
    cout << "Total events processed: " << eventToRows.size() << endl;
    cout << "Total JSON events written: " << nJsonEventsWritten << endl;
    cout << "Total JSON tracks written: " << nJsonTracksWritten << endl;
}
