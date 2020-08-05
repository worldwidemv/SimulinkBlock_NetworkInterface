//
// File: test_NetworkInterface_UDP_Output.cpp
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
#include "test_NetworkInterface_UDP_Output_dt.h"

// Block signals (auto storage)
BlockIO_test_NetworkInterface_U test_NetworkInterface_UDP_Out_B;

// Block states (auto storage)
D_Work_test_NetworkInterface_UD test_NetworkInterface_UDP_DWork;

// Real-time model
RT_MODEL_test_NetworkInterface_ test_NetworkInterface_UDP_Ou_M_;
RT_MODEL_test_NetworkInterface_ *const test_NetworkInterface_UDP_Ou_M =
  &test_NetworkInterface_UDP_Ou_M_;

//
// Writes out MAT-file header.  Returns success or failure.
// Returns:
//      0 - success
//      1 - failure
//
int_T rt_WriteMat4FileHeader(FILE *fp, int32_T m, int32_T n, const char *name)
{
  typedef enum { ELITTLE_ENDIAN, EBIG_ENDIAN } ByteOrder;

  int16_T one = 1;
  ByteOrder byteOrder = (*((int8_T *)&one)==1) ? ELITTLE_ENDIAN : EBIG_ENDIAN;
  int32_T type = (byteOrder == ELITTLE_ENDIAN) ? 0: 1000;
  int32_T imagf = 0;
  int32_T name_len = (int32_T)strlen(name) + 1;
  if ((fwrite(&type, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&m, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&n, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&imagf, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(&name_len, sizeof(int32_T), 1, fp) == 0) ||
      (fwrite(name, sizeof(char), name_len, fp) == 0)) {
    return(1);
  } else {
    return(0);
  }
}                                      // end rt_WriteMat4FileHeader

// Model step function
void test_NetworkInterface_UDP_Output_step(void)
{
  // local block i/o variables
  real_T rtb_LowLevelNetInterface_01_o1[6];
  real_T rtb_DigitalClock;
  real_T rtb_TmpSignalConversionAtToFile[7];
  int32_T i;

  // Delay: '<S1>/NetDelay'
  for (i = 0; i < 6; i++) {
    test_NetworkInterface_UDP_Out_B.NetDelay[i] =
      test_NetworkInterface_UDP_DWork.NetDelay_DSTATE[i];
  }

  // End of Delay: '<S1>/NetDelay'

  // DigitalClock: '<S1>/Digital Clock'
  rtb_DigitalClock = test_NetworkInterface_UDP_Ou_M->Timing.taskTime0;

  // S-Function (sfunc_NetworkInterface_01): '<S1>/LowLevel NetInterface_01'
  lctNetworkInterface_InputOutput
    (&test_NetworkInterface_UDP_DWork.LowLevelNetInterface_01_PWORK,
     test_NetworkInterface_UDP_Out_P.Constant0_Value, (real_T*)
     &test_NetworkInterface_UDP_Out_P.Constant0_Value, rtb_DigitalClock,
     &rtb_LowLevelNetInterface_01_o1[0],
     &test_NetworkInterface_UDP_Out_B.LowLevelNetInterface_01_o2);

  // Stop: '<Root>/Stop Simulation'
  if (test_NetworkInterface_UDP_Out_B.NetDelay[5] != 0.0) {
    rtmSetStopRequested(test_NetworkInterface_UDP_Ou_M, 1);
  }

  // End of Stop: '<Root>/Stop Simulation'

  // SignalConversion: '<Root>/TmpSignal ConversionAtTo FileInport1'
  for (i = 0; i < 6; i++) {
    rtb_TmpSignalConversionAtToFile[i] =
      test_NetworkInterface_UDP_Out_B.NetDelay[i];
  }

  rtb_TmpSignalConversionAtToFile[6] =
    test_NetworkInterface_UDP_Out_B.LowLevelNetInterface_01_o2;

  // End of SignalConversion: '<Root>/TmpSignal ConversionAtTo FileInport1'

  // ToFile: '<Root>/To File'
  {
    if (!(++test_NetworkInterface_UDP_DWork.ToFile_IWORK.Decimation % 1) &&
        (test_NetworkInterface_UDP_DWork.ToFile_IWORK.Count * (7 + 1)) + 1 <
        100000000 ) {
      FILE *fp = (FILE *) test_NetworkInterface_UDP_DWork.ToFile_PWORK.FilePtr;
      if (fp != (NULL)) {
        real_T u[7 + 1];
        test_NetworkInterface_UDP_DWork.ToFile_IWORK.Decimation = 0;
        u[0] = test_NetworkInterface_UDP_Ou_M->Timing.taskTime0;
        u[1] = rtb_TmpSignalConversionAtToFile[0];
        u[2] = rtb_TmpSignalConversionAtToFile[1];
        u[3] = rtb_TmpSignalConversionAtToFile[2];
        u[4] = rtb_TmpSignalConversionAtToFile[3];
        u[5] = rtb_TmpSignalConversionAtToFile[4];
        u[6] = rtb_TmpSignalConversionAtToFile[5];
        u[7] = rtb_TmpSignalConversionAtToFile[6];
        if (fwrite(u, sizeof(real_T), 7 + 1, fp) != 7 + 1) {
          rtmSetErrorStatus(test_NetworkInterface_UDP_Ou_M,
                            "Error writing to MAT-file SimulinkOutputData.mat");
          return;
        }

        if (((++test_NetworkInterface_UDP_DWork.ToFile_IWORK.Count) * (7 + 1))+1
            >= 100000000) {
          (void)fprintf(stdout,
                        "*** The ToFile block will stop logging data before\n"
                        "    the simulation has ended, because it has reached\n"
                        "    the maximum number of elements (100000000)\n"
                        "    allowed in MAT-file SimulinkOutputData.mat.\n");
        }
      }
    }
  }

  // Update for Delay: '<S1>/NetDelay'
  for (i = 0; i < 6; i++) {
    test_NetworkInterface_UDP_DWork.NetDelay_DSTATE[i] =
      rtb_LowLevelNetInterface_01_o1[i];
  }

  // End of Update for Delay: '<S1>/NetDelay'

  // External mode
  rtExtModeUploadCheckTrigger(1);

  {                                    // Sample time: [0.05s, 0.0s]
    rtExtModeUpload(0, test_NetworkInterface_UDP_Ou_M->Timing.taskTime0);
  }

  // signal main to stop simulation
  {                                    // Sample time: [0.05s, 0.0s]
    if ((rtmGetTFinal(test_NetworkInterface_UDP_Ou_M)!=-1) &&
        !((rtmGetTFinal(test_NetworkInterface_UDP_Ou_M)-
           test_NetworkInterface_UDP_Ou_M->Timing.taskTime0) >
          test_NetworkInterface_UDP_Ou_M->Timing.taskTime0 * (DBL_EPSILON))) {
      rtmSetErrorStatus(test_NetworkInterface_UDP_Ou_M, "Simulation finished");
    }

    if (rtmGetStopRequested(test_NetworkInterface_UDP_Ou_M)) {
      rtmSetErrorStatus(test_NetworkInterface_UDP_Ou_M, "Simulation finished");
    }
  }

  // Update absolute time for base rate
  // The "clockTick0" counts the number of times the code of this task has
  //  been executed. The absolute time is the multiplication of "clockTick0"
  //  and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
  //  overflow during the application lifespan selected.

  test_NetworkInterface_UDP_Ou_M->Timing.taskTime0 =
    (++test_NetworkInterface_UDP_Ou_M->Timing.clockTick0) *
    test_NetworkInterface_UDP_Ou_M->Timing.stepSize0;
}

// Model initialize function
void test_NetworkInterface_UDP_Output_initialize(void)
{
  // Registration code

  // initialize real-time model
  (void) memset((void *)test_NetworkInterface_UDP_Ou_M, 0,
                sizeof(RT_MODEL_test_NetworkInterface_));
  rtmSetTFinal(test_NetworkInterface_UDP_Ou_M, -1);
  test_NetworkInterface_UDP_Ou_M->Timing.stepSize0 = 0.05;

  // External mode info
  test_NetworkInterface_UDP_Ou_M->Sizes.checksums[0] = (2530923683U);
  test_NetworkInterface_UDP_Ou_M->Sizes.checksums[1] = (2108216961U);
  test_NetworkInterface_UDP_Ou_M->Sizes.checksums[2] = (1374556778U);
  test_NetworkInterface_UDP_Ou_M->Sizes.checksums[3] = (3887323639U);

  {
    static const sysRanDType rtAlwaysEnabled = SUBSYS_RAN_BC_ENABLE;
    static RTWExtModeInfo rt_ExtModeInfo;
    static const sysRanDType *systemRan[1];
    test_NetworkInterface_UDP_Ou_M->extModeInfo = (&rt_ExtModeInfo);
    rteiSetSubSystemActiveVectorAddresses(&rt_ExtModeInfo, systemRan);
    systemRan[0] = &rtAlwaysEnabled;
    rteiSetModelMappingInfoPtr(test_NetworkInterface_UDP_Ou_M->extModeInfo,
      &test_NetworkInterface_UDP_Ou_M->SpecialInfo.mappingInfo);
    rteiSetChecksumsPtr(test_NetworkInterface_UDP_Ou_M->extModeInfo,
                        test_NetworkInterface_UDP_Ou_M->Sizes.checksums);
    rteiSetTPtr(test_NetworkInterface_UDP_Ou_M->extModeInfo, rtmGetTPtr
                (test_NetworkInterface_UDP_Ou_M));
  }

  // block I/O
  (void) memset(((void *) &test_NetworkInterface_UDP_Out_B), 0,
                sizeof(BlockIO_test_NetworkInterface_U));

  // states (dwork)
  (void) memset((void *)&test_NetworkInterface_UDP_DWork, 0,
                sizeof(D_Work_test_NetworkInterface_UD));

  // data type transition information
  {
    static DataTypeTransInfo dtInfo;
    (void) memset((char_T *) &dtInfo, 0,
                  sizeof(dtInfo));
    test_NetworkInterface_UDP_Ou_M->SpecialInfo.mappingInfo = (&dtInfo);
    dtInfo.numDataTypes = 14;
    dtInfo.dataTypeSizes = &rtDataTypeSizes[0];
    dtInfo.dataTypeNames = &rtDataTypeNames[0];

    // Block I/O transition table
    dtInfo.BTransTable = &rtBTransTable;

    // Parameters transition table
    dtInfo.PTransTable = &rtPTransTable;
  }

  {
    int32_T i;

    // Start for S-Function (sfunc_NetworkInterface_01): '<S1>/LowLevel NetInterface_01' 
    lctNetworkInterface_Initialise
      (&test_NetworkInterface_UDP_DWork.LowLevelNetInterface_01_PWORK, 1U, 6U, 2,
       (uint32_T*)test_NetworkInterface_UDP_Out_P.LowLevelNetInterface_01_p3, 6,
       (real_T*)test_NetworkInterface_UDP_Out_P.LowLevelNetInterface_01_p4,
       test_NetworkInterface_UDP_Out_P.LowLevelNetInterface_01_p5,
       test_NetworkInterface_UDP_Out_P.LowLevelNetInterface_01_p6,
       test_NetworkInterface_UDP_Out_P.LowLevelNetInterface_01_p7, (int8_T*)
       test_NetworkInterface_UDP_Out_P.LowLevelNetInterface_01_p8,
       test_NetworkInterface_UDP_Out_P.LowLevelNetInterface_01_p9);

    // Start for ToFile: '<Root>/To File'
    {
      FILE *fp = (NULL);
      char fileName[509] = "SimulinkOutputData.mat";
      if ((fp = fopen(fileName, "wb")) == (NULL)) {
        rtmSetErrorStatus(test_NetworkInterface_UDP_Ou_M,
                          "Error creating .mat file SimulinkOutputData.mat");
        return;
      }

      if (rt_WriteMat4FileHeader(fp, 7 + 1, 0, "data")) {
        rtmSetErrorStatus(test_NetworkInterface_UDP_Ou_M,
                          "Error writing mat file header to file SimulinkOutputData.mat");
        return;
      }

      test_NetworkInterface_UDP_DWork.ToFile_IWORK.Count = 0;
      test_NetworkInterface_UDP_DWork.ToFile_IWORK.Decimation = -1;
      test_NetworkInterface_UDP_DWork.ToFile_PWORK.FilePtr = fp;
    }

    // InitializeConditions for Delay: '<S1>/NetDelay'
    for (i = 0; i < 6; i++) {
      test_NetworkInterface_UDP_DWork.NetDelay_DSTATE[i] =
        test_NetworkInterface_UDP_Out_P.NetDelay_InitialCondition;
    }

    // End of InitializeConditions for Delay: '<S1>/NetDelay'
  }
}

// Model terminate function
void test_NetworkInterface_UDP_Output_terminate(void)
{
  // Terminate for S-Function (sfunc_NetworkInterface_01): '<S1>/LowLevel NetInterface_01' 
  lctNetworkInterface_Deinitialise
    (&test_NetworkInterface_UDP_DWork.LowLevelNetInterface_01_PWORK);

  // Terminate for ToFile: '<Root>/To File'
  {
    FILE *fp = (FILE *) test_NetworkInterface_UDP_DWork.ToFile_PWORK.FilePtr;
    if (fp != (NULL)) {
      char fileName[509] = "SimulinkOutputData.mat";
      if (fclose(fp) == EOF) {
        rtmSetErrorStatus(test_NetworkInterface_UDP_Ou_M,
                          "Error closing MAT-file SimulinkOutputData.mat");
        return;
      }

      if ((fp = fopen(fileName, "r+b")) == (NULL)) {
        rtmSetErrorStatus(test_NetworkInterface_UDP_Ou_M,
                          "Error reopening MAT-file SimulinkOutputData.mat");
        return;
      }

      if (rt_WriteMat4FileHeader(fp, 7 + 1,
           test_NetworkInterface_UDP_DWork.ToFile_IWORK.Count, "data")) {
        rtmSetErrorStatus(test_NetworkInterface_UDP_Ou_M,
                          "Error writing header for data to MAT-file SimulinkOutputData.mat");
      }

      if (fclose(fp) == EOF) {
        rtmSetErrorStatus(test_NetworkInterface_UDP_Ou_M,
                          "Error closing MAT-file SimulinkOutputData.mat");
        return;
      }

      test_NetworkInterface_UDP_DWork.ToFile_PWORK.FilePtr = (NULL);
    }
  }
}

//
// File trailer for generated code.
//
// [EOF]
//
