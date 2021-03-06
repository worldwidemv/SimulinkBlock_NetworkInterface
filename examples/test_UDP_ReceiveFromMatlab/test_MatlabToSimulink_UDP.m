%% Example Script for sending data to the Simulink block NetworkInterface
%
%% To run this example:
% 1. open 'test_NetworkInterface_UDP_Output.slx' in Matlab and compile the Simulink diagram
% 2. run THIS Matlab script, this script will also execute and stop the Simulink executable!!
%
%% This script will:
% 1. start the executable './test_NetworkInterface_UDP_Output'
% 2. connect to the Simulink block
% 3. send data to the Simulink block for 30 seconds 
% 4. close the connection to the Simulink block
% 5. stop the executable

%% implementation
% #########################################################################
disp('UDP Matlab -> Simulink example');
disp('#########################################');

% does the executable exist?
if (~exist('./test_NetworkInterface_UDP_Output', 'file'))
    error(['The executable ''./test_NetworkInterface_UDP_Output'' was not found in ', char(10), pwd(), char(10), char(10), ...
           'Please open ''test_NetworkInterface_UDP_Output.slx'' in Simulink and compile the Simulink diagram.']);
end

% cleanup
if (exist('SimulinkOutputData.mat', 'file'))
    disp(' -> deleting old ''SimulinkOutputData.mat''' );
    delete('SimulinkOutputData.mat');
end

% start the executable
disp(' -> starting the executable');
command = ['sudo ls'];
system(command);
command = ['cd ', pwd(), '; sudo ./test_NetworkInterface_UDP_Output &'];
[status,cmdout] = system(command);
pause(1);

% close any existing connection from an earlier run
if (exist('con', 'var'))
    fclose(con);
    clear con;
end

% setup for the UDP connection
% IP and ports -> has to match the Simulink block setup!
con = udp('127.0.0.1', 20000, 'LocalPort', 20001);
% set byte order, since the default is bigEndian!!!
con.ByteOrder = 'littleEndian';


% data to send
data = double([0,  [200 13], [1 -1], 0]); 

% open the connection
disp(' -> open connection');
fopen(con);

disp(' -> sending data for 30 seconds ...');
% run this loop for 30 seconds
for i = 1:600
    pause(0.05);
    % send data
    data(1) = i;
    ni_sendData(con, data);
end

% stop the executable
disp(' -> stopping the executable');
data(6) = 1;
ni_sendData(con, data);

% close the connection
fclose(con);
disp(' -> connection closed');
disp('#########################################');
disp(char(10));

% load and display simulink data
disp(' -> loading ''SimulinkOutputData.mat''' );
pause(2);
simData = load('SimulinkOutputData.mat');
d = simData.data;
time = d(1,:);
out1 = d(2,:);
out2 = d(3:4,:);
out3 = d(5:6,:);
out4 = d(7,:);
msgCounter = d(8,:);

% plots
disp(' -> ploting Simulink data' );
figure();
subplot(4,1,1);
plot(time,out1); legend('OUT 1');
subplot(4,1,2);
plot(time,out2); legend('OUT 2');
subplot(4,1,3);
plot(time,out3); legend('OUT 3');
subplot(4,1,4);
plot(time,out4); legend('OUT 4');