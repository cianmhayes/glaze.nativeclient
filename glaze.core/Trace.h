#pragma once

void InitializeTracing();

void TraceFrameCaptureStart(int frameNumber);

void TraceFrameCaptureEnd(int frameNumber);

void TraceFrameReceived(int frameNumber);

void TraceFrameGenerated();

void TraceDispatchCycleStart();

void TraceFrameDispatched();

void TraceDrawStart(int frameNumber);

void TraceDrawEnd(int frameNumber);

void TraceMissingFrame(int frameNumber);

void TraceUpdateCanvas();

void TraceError(int hresult);