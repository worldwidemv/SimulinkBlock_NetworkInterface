%% Example Script for receiving data from Simulink block NetworkInterface
%
%% To run this example:
% 1. open 'test_NetworkInterface_UDP_Input.slx' in Matlab and compile the Simulink diagram
% 2. execute the compiled program -> 'sudo ./test_NetworkInterface_UDP_Input'
% 3. run THIS Matlab script (multiple time is also fine)
% 4. stop the program './test_NetworkInterface_UDP_Input' with CONTR-C
%
%% This script will:
% 1. connect to the Simulink block
% 2. receive at least 100 dataset's -> 5 seconds 
% 3. close the connection to the Simulink block

%% implementation
% #########################################################################
disp('UDP Simulink -> Maltab example');
disp('#########################################');


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
% make sure the input buffer is big enough
con.InputBufferSize = 10000;

% set the receive callback function, so no data package is missed
con.DatagramReceivedFcn = 'ni_receiveDataUDP(con);';
% the received data is stored in the global variable 'res' -> setup this variable
global res; res = struct();


% open the connection
disp(' -> open connection');
fopen(con);
tic;

disp(' -> start receiving data');
% run this loop for 30 seconds at most, but break the loop as soon as 100 packages are received
for i = 1:600
    pause(0.05);
    % Did we receive enough data?
    if (length(res) >= 100)
        % yes -> abort the loop
        break;
    end
end

timeNeeded = toc;
fclose(con);
disp(' -> connection closed');
disp([' -> received ', num2str(length(res)), ' dataset''s in ', num2str(timeNeeded), ' seconds']);
disp('#########################################');
disp(char(10));

disp('''res'' variable:');
disp(res);

disp('data of the last receiced package (''res(end).data''):');
disp(res(end).data);
