#ifndef _DEventProcessor_TrackMLextract_h_
#define _DEventProcessor_TrackMLextract_h_
#include <JANA/JEventProcessor.h>
#include <JANA/JEvent.h>
#include <TTree.h>
#include <vector>
#include <memory>
#include <map>

class DEventProcessor_TrackMLextract : public JEventProcessor {
public:
    DEventProcessor_TrackMLextract();
    ~DEventProcessor_TrackMLextract();
    const char* className(void) const { return "DEventProcessor_TrackMLextract"; }
    void Init(void);
    void BeginRun(const std::shared_ptr<const JEvent>& event);
    void Process(const std::shared_ptr<const JEvent>& event);
    void EndRun(void);
    void Finish(void);
private:
    TTree *t_out;
    // Tree branch variables
    int m_eventID;
    int m_trackID;
    int m_pid;
    int m_nHitsFDC;
    unsigned int m_measuredFDCHits;
    unsigned int m_potentialFDCHits;
    unsigned int m_FDCPlanesBitmask;
    float m_trackE;
    float m_trackP;
    float m_trackPhi;
    float m_trackTheta;
    std::vector<int> fdc_layer;
    std::vector<int> fdc_wire;
    std::vector<float> fdc_time;
    std::vector<float> fdc_charge;
    std::vector<float> fdc_x;
    std::vector<float> fdc_y;
    std::vector<float> fdc_z;
    std::vector<int> fdc_truth_track;
    int m_mcTrackID;
    float m_truthPurity;
    float m_match_px;
    float m_match_py;
    float m_match_pz;
    float m_match_E;
    void ClearVectors();
};
#endif
