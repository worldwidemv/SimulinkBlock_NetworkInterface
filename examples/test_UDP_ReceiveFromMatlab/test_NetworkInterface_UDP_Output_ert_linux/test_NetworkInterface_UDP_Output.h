//
// File: test_NetworkInterface_UDP_Output.h
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
#ifndef RTW_HEADER_test_NetworkInterface_UDP_Output_h_
#define RTW_HEADER_test_NetworkInterface_UDP_Output_h_
#include <stddef.h>
#include <float.h>
#include <string.h>
#ifndef test_NetworkInterface_UDP_Output_COMMON_INCLUDES_
# define test_NetworkInterface_UDP_Output_COMMON_INCLUDES_
#include <stdio.h>
#include <string.h>
#include "rtwtypes.h"
#include "rtw_extmode.h"
#include "sysran_types.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#include "dt_info.h"
#include "ext_work.h"
#include "NetworkInterface_01.hpp"
#include "UDPHandle.hpp"
#include "TCPHandle.hpp"
#endif                                 // test_NetworkInterface_UDP_Output_COMMON_INCLUDES_ 

#include "test_NetworkInterface_UDP_Output_types.h"

// Shared type includes
#include "multiword_types.h"

// Macros for accessing real-time model data structure
#ifndef rtmGetFinalTime
# define rtmGetFinalTime(rtm)          ((rtm)->Timing.tFinal)
#endif

#ifndef rtmGetRTWExtModeInfo
# define rtmGetRTWExtModeInfo(rtm)     ((rtm)->extModeInfo)
#endif

#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((rtm)->Timing.stopRequestedFlag)
#endif

#ifndef rtmSetStopRequested
# define rtmSetStopRequested(rtm, val) ((rtm)->Timing.stopRequestedFlag = (val))
#endif

#ifndef rtmGetStopRequestedPtr
# define rtmGetStopRequestedPtr(rtm)   (&((rtm)->Timing.stopRequestedFlag))
#endif

#ifndef rtmGetT
# define rtmGetT(rtm)                  ((rtm)->Timing.taskTime0)
#endif

#ifndef rtmGetTFinal
# define rtmGetTFinal(rtm)             ((rtm)->Timing.tFinal)
#endif

#define test_NetworkInterface_UDP_Output_M (test_NetworkInterface_UDP_Ou_M)

// Block signals (auto storage)
typedef struct {
  real_T NetDelay[6];                  // '<S1>/NetDelay'
  real_T LowLevelNetInterface_01_o2;   // '<S1>/LowLevel NetInterface_01'
} BlockIO_test_NetworkInterface_U;

// Block states (auto storage) for system '<Root>'
typedef struct {
  real_T NetDelay_DSTATE[6];           // '<S1>/NetDelay'
  void *LowLevelNetInterface_01_PWORK; // '<S1>/LowLevel NetInterface_01'
  struct {
    void *LoggedData[5];
  } Scope3_PWORK;                      // '<Root>/Scope3'

  struct {
    void *FilePtr;
  } ToFile_PWORK;                      // '<Root>/To File'

  struct {
    int_T Count;
    int_T Decimation;
  } ToFile_IWORK;                      // '<Root>/To File'
} D_Work_test_NetworkInterface_UD;

// Parameters (auto storage)
struct Parameters_test_NetworkInterfac_ {
  real_T NetDelay_InitialCondition;    // Expression: 0
                                       //  Referenced by: '<S1>/NetDelay'

  real_T Constant0_Value;              // Expression: 0
                                       //  Referenced by: '<Root>/Constant0'

  real_T LowLevelNetInterface_01_p4[6];// Expression: initialOutputValues
                                       //  Referenced by: '<S1>/LowLevel NetInterface_01'

  int32_T LowLevelNetInterface_01_p5;  // Expression: useUDP
                                       //  Referenced by: '<S1>/LowLevel NetInterface_01'

  int32_T LowLevelNetInterface_01_p6;  // Expression: startConnection
                                       //  Referenced by: '<S1>/LowLevel NetInterface_01'

  uint32_T NetDelay_DelayLength;       // Computed Parameter: NetDelay_DelayLength
                                       //  Referenced by: '<S1>/NetDelay'

  uint32_T LowLevelNetInterface_01_p3[2];// Expression: blockOptions
                                         //  Referenced by: '<S1>/LowLevel NetInterface_01'

  uint32_T LowLevelNetInterface_01_p7; // Expression: localPort
                                       //  Referenced by: '<S1>/LowLevel NetInterface_01'

  uint32_T LowLevelNetInterface_01_p9; // Expression: remotePort
                                       //  Referenced by: '<S1>/LowLevel NetInterface_01'

  int8_T LowLevelNetInterface_01_p8[10];// Computed Parameter: LowLevelNetInterface_01_p8
                                        //  Referenced by: '<S1>/LowLevel NetInterface_01'

};

// Real-time Model Data Structure
struct tag_RTM_test_NetworkInterface_U {
  const char_T *errorStatus;
  RTWExtModeInfo *extModeInfo;

  //
  //  Sizes:
  //  The following substructure contains sizes information
  //  for many of the model attributes such as inputs, outputs,
  //  dwork, sample times, etc.

  struct {
    uint32_T checksums[4];
  } Sizes;

  //
  //  SpecialInfo:
  //  The following substructure contains special information
  //  related to other components that are dependent on RTW.

  struct {
    const void *mappingInfo;
  } SpecialInfo;

  //
  //  Timing:
  //  The following substructure contains information regarding
  //  the timing information for the model.

  struct {
    time_T taskTime0;
    uint32_T clockTick0;
    time_T stepSize0;
    time_T tFinal;
    boolean_T stopRequestedFlag;
  } Timing;
};

// Block parameters (auto storage)
#ifdef __cplusplus

extern "C" {

#endif

  extern Parameters_test_NetworkInterfac test_NetworkInterface_UDP_Out_P;

#ifdef __cplusplus

}
#endif

// Block signals (auto storage)
extern BlockIO_test_NetworkInterface_U test_NetworkInterface_UDP_Out_B;

// Block states (auto storage)
extern D_Work_test_NetworkInterface_UD test_NetworkInterface_UDP_DWork;

#ifdef __cplusplus

extern "C" {

#endif

#ifdef __cplusplus

}
#endif

#ifdef __cplusplus

extern "C" {

#endif

  // Model entry point functions
  extern void test_NetworkInterface_UDP_Output_initialize(void);
  extern void test_NetworkInterface_UDP_Output_step(void);
  extern void test_NetworkInterface_UDP_Output_terminate(void);

#ifdef __cplusplus

}
#endif

// Real-time Model object
#ifdef __cplusplus

extern "C" {

#endif

  extern RT_MODEL_test_NetworkInterface_ *const test_NetworkInterface_UDP_Ou_M;

#ifdef __cplusplus

}
#endif

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'test_NetworkInterface_UDP_Output'
//  '<S1>'   : 'test_NetworkInterface_UDP_Output/NetworkInterface_01#1'

#endif                                 // RTW_HEADER_test_NetworkInterface_UDP_Output_h_ 

//
// File trailer for generated code.
//
// [EOF]
//
