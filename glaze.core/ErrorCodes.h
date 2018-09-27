#pragma once

#define SEVERITY_ERROR          0x80000000

#define FACILITY_GLAZE          607 << 16

#define MAKE_GLAZE_ERROR(CODE)  SEVERITY_ERROR | FACILITY_GLAZE | CODE

//==========================================================================================
// MF ERRORS
//==========================================================================================

#define GLAZE_ERROR_UNSUPPORTED_PIXEL_FORMAT    MAKE_GLAZE_ERROR(1)

#define GLAZE_ERROR_INNACCESSIBLE_BUFFER        MAKE_GLAZE_ERROR(2)

