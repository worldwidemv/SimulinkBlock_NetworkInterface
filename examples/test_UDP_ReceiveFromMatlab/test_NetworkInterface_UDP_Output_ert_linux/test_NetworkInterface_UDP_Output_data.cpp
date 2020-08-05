//
// File: test_NetworkInterface_UDP_Output_data.cpp
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
#include "test_NetworkInterface_UDP_Output.h"
#include "test_NetworkInterface_UDP_Output_private.h"

// Block parameters (auto storage)
Parameters_test_NetworkInterfac test_NetworkInterface_UDP_Out_P = {
  0.0,                                 // Expression: 0
                                       //  Referenced by: '<S1>/NetDelay'

  0.0,                                 // Expression: 0
                                       //  Referenced by: '<Root>/Constant0'


  //  Expression: initialOutputValues
  //  Referenced by: '<S1>/LowLevel NetInterface_01'

  { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
  1,                                   // Expression: useUDP
                                       //  Referenced by: '<S1>/LowLevel NetInterface_01'

  1,                                   // Expression: startConnection
                                       //  Referenced by: '<S1>/LowLevel NetInterface_01'

  1U,                                  // Computed Parameter: NetDelay_DelayLength
                                       //  Referenced by: '<S1>/NetDelay'


  //  Expression: blockOptions
  //  Referenced by: '<S1>/LowLevel NetInterface_01'

  { 1U, 0U },
  20000U,                              // Expression: localPort
                                       //  Referenced by: '<S1>/LowLevel NetInterface_01'

  20001U,                              // Expression: remotePort
                                       //  Referenced by: '<S1>/LowLevel NetInterface_01'


  //  Computed Parameter: LowLevelNetInterface_01_p8
  //  Referenced by: '<S1>/LowLevel NetInterface_01'

  { 49, 50, 55, 46, 48, 46, 48, 46, 49, 0 }
};

//
// File trailer for generated code.
//
// [EOF]
//
