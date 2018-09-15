#include "pch.h"
#include "Trace.h"
#include <Windows.h>
#include <TraceLoggingProvider.h> 

using namespace std;

TRACELOGGING_DECLARE_PROVIDER(g_hGlazeProvider);

// Define the GUID to use in TraceLoggingProviderRegister 
// {AE9D9DDF-088B-45CB-8FFA-573433DE73C2}
TRACELOGGING_DEFINE_PROVIDER(
    g_hGlazeProvider,
    "Glaze",
    (0xae9d9ddf, 0x88b, 0x45cb, 0x8f, 0xfa, 0x57, 0x34, 0x33, 0xde, 0x73, 0xc2));


void InitializeTracing()
{
    TraceLoggingRegister(g_hGlazeProvider);
}

void TraceFrameGenerated()
{
    TraceLoggingWrite(g_hGlazeProvider, "FrameGenerated");
}

void TraceDispatchCycleStart()
{
    TraceLoggingWrite(g_hGlazeProvider, "DispatchCycleStart");
}

void TraceFrameDispatched()
{
    TraceLoggingWrite(g_hGlazeProvider, "FrameDispatched");
}

void TraceDrawStart()
{
    TraceLoggingWrite(g_hGlazeProvider, "DrawStart");
}

void TraceDrawEnd()
{
    TraceLoggingWrite(g_hGlazeProvider, "DrawEnd");
}

void TraceMissingFrame()
{
    TraceLoggingWrite(g_hGlazeProvider, "MissingFrame");
}

void TraceUpdateCanvas()
{
    TraceLoggingWrite(g_hGlazeProvider, "UpdateCanvas");
}