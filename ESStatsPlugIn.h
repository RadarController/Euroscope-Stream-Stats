#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>      // defines RECT, NULL, etc.

#include <EuroScopePlugIn.h>

#include <string>
#include <unordered_set>
#include <ctime>

class CESStatsPlugIn final : public EuroScopePlugIn::CPlugIn
{
public:
    CESStatsPlugIn();
    ~CESStatsPlugIn() override = default;

    void OnRadarTargetPositionUpdate(EuroScopePlugIn::CRadarTarget RadarTarget) override;
    void OnFlightPlanFlightPlanDataUpdate(EuroScopePlugIn::CFlightPlan FlightPlan) override;
    void OnFlightPlanDisconnect(EuroScopePlugIn::CFlightPlan FlightPlan) override;

private:
    void TickIfDue();
    void RecomputeCounts();
    void WriteOutputs();

    bool IsAssumedByMe(const EuroScopePlugIn::CFlightPlan& fp) const;

private:
    int m_assumedNow = 0;
    int m_handledSession = 0;

    std::unordered_set<std::string> m_countedCallsigns;
    std::time_t m_lastTick = 0;
};
