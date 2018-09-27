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

void TraceFrameCaptureStart(int frameNumber)
{
    TraceLoggingWrite(
        g_hGlazeProvider,
        "FrameCaptureStart",
        TraceLoggingInt32(frameNumber, "frameNumber"));
}

void TraceFrameCaptureEnd(int frameNumber)
{
    TraceLoggingWrite(
        g_hGlazeProvider,
        "FrameCaptureEnd",
        TraceLoggingInt32(frameNumber, "frameNumber"));
}

void TraceFrameReceived(int frameNumber)
{
    TraceLoggingWrite(
        g_hGlazeProvider,
        "FrameReceived",
        TraceLoggingInt32(frameNumber, "frameNumber"));
}

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

void TraceDrawStart(int frameNumber)
{
    TraceLoggingWrite(
        g_hGlazeProvider,
        "DrawStart",
        TraceLoggingInt32(frameNumber, "frameNumber"));
}

void TraceDrawEnd(int frameNumber)
{
    TraceLoggingWrite(
        g_hGlazeProvider,
        "DrawEnd",
        TraceLoggingInt32(frameNumber, "frameNumber"));
}

void TraceMissingFrame(int frameNumber)
{
    TraceLoggingWrite(
        g_hGlazeProvider,
        "MissingFrame",
        TraceLoggingInt32(frameNumber, "frameNumber"));
}

void TraceUpdateCanvas()
{
    TraceLoggingWrite(g_hGlazeProvider, "UpdateCanvas");
}

void TraceError(int hresult)
{
    TraceLoggingWrite(g_hGlazeProvider, "Exception", TraceLoggingHexInt32(hresult, "hresult"));
}