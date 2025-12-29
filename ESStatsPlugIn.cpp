#include "ESStatsPlugIn.h"

#include <shlobj.h>
#include <fstream>
#include <algorithm>

static std::string ToUpper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return (char)std::toupper(c); });
    return s;
}

static std::string GetDocumentsFolder()
{
    char path[MAX_PATH] = { 0 };
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path)))
        return std::string(path);
    return ".";
}

static void EnsureDir(const std::string& dir)
{
    CreateDirectoryA(dir.c_str(), NULL); // OK if exists
}

CESStatsPlugIn::CESStatsPlugIn()
    : EuroScopePlugIn::CPlugIn(
        EuroScopePlugIn::COMPATIBILITY_CODE,
        "ES Stats Output (Assumed + Handled)",
        "1.0.0",
        "ChatRadarController",
        "Writes assumed-now + handled-session to Documents\\ESStats")
{
}

void CESStatsPlugIn::OnRadarTargetPositionUpdate(EuroScopePlugIn::CRadarTarget /*RadarTarget*/)
{
    TickIfDue();
}

void CESStatsPlugIn::OnFlightPlanFlightPlanDataUpdate(EuroScopePlugIn::CFlightPlan /*FlightPlan*/)
{
    TickIfDue();
}

void CESStatsPlugIn::OnFlightPlanDisconnect(EuroScopePlugIn::CFlightPlan /*FlightPlan*/)
{
    TickIfDue();
}

void CESStatsPlugIn::TickIfDue()
{
    const std::time_t now = std::time(nullptr);
    if (now == m_lastTick) return; // throttle to 1/sec
    m_lastTick = now;

    RecomputeCounts();
    WriteOutputs();
}

bool CESStatsPlugIn::IsAssumedByMe(const EuroScopePlugIn::CFlightPlan& fp) const
{
    // Your header exposes both GetState() and GetFPState().
    // Either is fine; we’ll use GetState().
    const int st = fp.GetState();

    if (st != EuroScopePlugIn::FLIGHT_PLAN_STATE_ASSUMED)
        return false;

    // This is the clean “is it mine?” check in your header.
    return fp.GetTrackingControllerIsMe();
}

void CESStatsPlugIn::RecomputeCounts()
{
    int assumed = 0;

    for (EuroScopePlugIn::CFlightPlan fp = FlightPlanSelectFirst();
        fp.IsValid();
        fp = FlightPlanSelectNext(fp))
    {
        if (!fp.IsValid()) continue;

        if (IsAssumedByMe(fp))
        {
            assumed++;

            const char* cs = fp.GetCallsign();
            std::string callsign = ToUpper(cs ? std::string(cs) : std::string());

            if (!callsign.empty() && m_countedCallsigns.insert(callsign).second)
                m_handledSession++;
        }
    }

    m_assumedNow = assumed;
}

void CESStatsPlugIn::WriteOutputs()
{
    const std::string base = GetDocumentsFolder() + "\\ESStats";
    EnsureDir(base);

    {
        std::ofstream f(base + "\\assumed_now.txt", std::ios::trunc);
        f << m_assumedNow;
    }
    {
        std::ofstream f(base + "\\handled_session.txt", std::ios::trunc);
        f << m_handledSession;
    }
}
