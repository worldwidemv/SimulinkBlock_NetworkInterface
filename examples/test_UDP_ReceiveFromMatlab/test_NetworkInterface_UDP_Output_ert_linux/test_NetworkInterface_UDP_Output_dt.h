//
//  test_NetworkInterface_UDP_Output_dt.h
//
//  Code generation for model "test_NetworkInterface_UDP_Output".
//
//  Model version              : 1.427
//  Simulink Coder version : 8.11 (R2016b) 25-Aug-2016
//  C++ source code generated on : Mon May 11 21:33:43 2020
//
//  Target selection: ert_linux.tlc
//  Embedded hardware selection: 32-bit Generic
//  Code generation objectives: Unspecified
//  Validation result: Not run


#include "ext_types.h"

// data type size table
static uint_T rtDataTypeSizes[] = {
  sizeof(real_T),
  sizeof(real32_T),
  sizeof(int8_T),
  sizeof(uint8_T),
  sizeof(int16_T),
  sizeof(uint16_T),
  sizeof(int32_T),
  sizeof(uint32_T),
  sizeof(boolean_T),
  sizeof(fcn_call_T),
  sizeof(int_T),
  sizeof(pointer_T),
  sizeof(action_T),
  2*sizeof(uint32_T)
};

// data type name table
static const char_T * rtDataTypeNames[] = {
  "real_T",
  "real32_T",
  "int8_T",
  "uint8_T",
  "int16_T",
  "uint16_T",
  "int32_T",
  "uint32_T",
  "boolean_T",
  "fcn_call_T",
  "int_T",
  "pointer_T",
  "action_T",
  "timer_uint32_pair_T"
};

// data type transitions for block I/O structure
static DataTypeTransition rtBTransitions[] = {
  { (char_T *)(&test_NetworkInterface_UDP_Out_B.NetDelay[0]), 0, 0, 7 }
  ,

  { (char_T *)(&test_NetworkInterface_UDP_DWork.NetDelay_DSTATE[0]), 0, 0, 6 },

  { (char_T *)(&test_NetworkInterface_UDP_DWork.LowLevelNetInterface_01_PWORK),
    11, 0, 7 },

  { (char_T *)(&test_NetworkInterface_UDP_DWork.ToFile_IWORK.Count), 10, 0, 1 }
};

// data type transition table for block I/O structure
static DataTypeTransitionTable rtBTransTable = {
  4U,
  rtBTransitions
};

// data type transitions for Parameters structure
static DataTypeTransition rtPTransitions[] = {
  { (char_T *)(&test_NetworkInterface_UDP_Out_P.NetDelay_InitialCondition), 0, 0,
    8 },

  { (char_T *)(&test_NetworkInterface_UDP_Out_P.LowLevelNetInterface_01_p5), 6,
    0, 2 },

  { (char_T *)(&test_NetworkInterface_UDP_Out_P.NetDelay_DelayLength), 7, 0, 5 },

  { (char_T *)(&test_NetworkInterface_UDP_Out_P.LowLevelNetInterface_01_p8[0]),
    2, 0, 10 }
};

// data type transition table for Parameters structure
static DataTypeTransitionTable rtPTransTable = {
  4U,
  rtPTransitions
};

// [EOF] test_NetworkInterface_UDP_Output_dt.h
