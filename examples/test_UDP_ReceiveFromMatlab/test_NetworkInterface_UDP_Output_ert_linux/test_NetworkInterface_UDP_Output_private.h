//
// File: test_NetworkInterface_UDP_Output_private.h
//
// Code generated for Simulink model 'test_NetworkInterface_UDP_Output'.
//
// Model version                  : 1.427
// Simulink Coder version         : 8.11 (R2016b) 25-Aug-2016
// C/C++ source code generated on : Mon May 11 21:33:43 2020
//
// Target selection: ert_linux.tlc
// Embedded hardware selection: 32-bit Generic
// Code generation objectives: Unspecified
// Validation result: Not run
//
#ifndef RTW_HEADER_test_NetworkInterface_UDP_Output_private_h_
#define RTW_HEADER_test_NetworkInterface_UDP_Output_private_h_
#include "rtwtypes.h"
#include "multiword_types.h"

// Private macros used by the generated code to access rtModel
#ifndef rtmSetTFinal
# define rtmSetTFinal(rtm, val)        ((rtm)->Timing.tFinal = (val))
#endif

#ifndef rtmGetTPtr
# define rtmGetTPtr(rtm)               (&(rtm)->Timing.taskTime0)
#endif

int_T rt_WriteMat4FileHeader(FILE *fp,
  int32_T m,
  int32_T n,
  const char_T *name);

#endif                                 // RTW_HEADER_test_NetworkInterface_UDP_Output_private_h_ 

//
// File trailer for generated code.
//
// [EOF]
//
