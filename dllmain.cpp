#include <cstddef>
#include <EuroScopePlugIn.h>
#include "ESStatsPlugIn.h"

static EuroScopePlugIn::CPlugIn* gPlugIn = nullptr;

void __declspec(dllexport) EuroScopePlugInInit(EuroScopePlugIn::CPlugIn** ppPlugInInstance)
{
    gPlugIn = new CESStatsPlugIn();
    *ppPlugInInstance = gPlugIn;
}

// SAFEST: don't delete here to avoid “double delete” differences between EuroScope builds.
// (Worst case: tiny leak on unload; best case: avoids crash.)
void __declspec(dllexport) EuroScopePlugInExit(void)
{
    // If you later confirm your ES build expects you to delete, change to:
    // delete gPlugIn; gPlugIn = nullptr;
}
