%% LCT build Blocks - NetworkInterface

%   TU Berlin --- Fachgebiet Regelungssystem
%   Author: Markus Valtin
%   Copyright © 2020 Markus Valtin. All rights reserved.
%
%   This program is free software: you can redistribute it and/or modify it under the terms of the 
%   GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.
%
%   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

%% disable specific warnings
warn_state = warning();
warning('off','MATLAB:mex:GccVersion_link');
warning('off','MATLAB:MKDIR:DirectoryExists');

%% check if the Soft Realtime Toolbox is available
if (~exist('xsrt_buildLibScriptInitLCT', 'file')), error(['The Soft-Realtime Simulink Toolbox is not availabe!', char(10), char(10), ...
        'This Simulink block depends on the "Soft-Realtime Simulink Toolbox", a universal framework for', char(10), 'creating own Simulink blocks (e.g. for own hardware) and soft realtime execution of Simulink diagrams.', char(10), char(10), ...
        'Please install the Soft-Realtime Simulink Toolbox from https://github.com/worldwidemv/SimulinkToolchain', char(10), 'and make sure the Matlab path is setup correctly, e.g. by running the Matlab script "srt_InstallSRT.m"!']);
end

%% check if the installation was done
if (~exist('.installDone', 'file')), error(['The installation was not done yet!', char(10), 'Please run the Matlab script "srt_InstallSRT.m", which is part of the Soft Realtime Toolbox and on which these block depends on!']); end

%% initialize the Matlab/SRT variables
[startDir, libDir, sfuncFolderName, srtAddPath, defs] = xsrt_buildLibScriptInitLCT(mfilename('fullpath'));

%% generate configuration for NetworkInterface blocks
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
libName = 'NetworkInterface';
%% NetworkInterface_01 block
def = legacy_code('initialize');
def.SFunctionName = 'sfunc_NetworkInterface_01';
% sfunction parameter
% numberOfInputs, numberOfOutputs, blockOptions, initialOutputValues, useUDP, startConnection, localPort, remoteIP, remotePort, sampleTime
def.StartFcnSpec  =    'void lctNetworkInterface_Initialise(   void **work1, uint32 p1, uint32 p2, uint32 size(p3,1), uint32 p3[], uint32 size(p4,1), double p4[], int32 p5, int32 p6, uint32 p7, int8 p8[], uint32 p9 )';
def.OutputFcnSpec =    'void lctNetworkInterface_InputOutput(  void **work1, double u1, double u2[p1], double u3, double y1[p2], double y2[1])';
def.TerminateFcnSpec = 'void lctNetworkInterface_Deinitialise( void **work1 )';
def.IncPaths     = {fullfile(pwd, 'inc')};
def.SrcPaths     = {fullfile(pwd, 'src')};
def.HeaderFiles  = {'NetworkInterface_01.hpp','UDPHandle.hpp','TCPHandle.hpp'};
def.SourceFiles  = {'NetworkInterface_01.cpp','UDPHandle.cpp','TCPHandle.cpp'};
def.LibPaths     = {'/lib/x86_64-linux-gnu', '/usr/lib/x86_64-linux-gnu'};
def.HostLibFiles   = {'libboost_system.so','libboost_thread.so','libboost_signals.so'};
def.TargetLibFiles = {'libboost_system.so','libboost_thread.so','libboost_signals.so'};
def.Options.language = 'C++';
def.Options.useTlcWithAccel = false;
def.SampleTime   = 'parameterized';
defs{end+1} = def;
srtAddPath{end+1} = fullfile(pwd, 'scripts'); 


%% auto generate the Simulink S-functions via LCT
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
result = xsrt_buildLibScriptEndLCT(startDir, libDir, libName, sfuncFolderName, srtAddPath, defs);
% copy the Simulink lib
copyfile(['.', filesep, '*.slx'], ['.', filesep, sfuncFolderName]);

%% restore the warning settings
warning(warn_state);
