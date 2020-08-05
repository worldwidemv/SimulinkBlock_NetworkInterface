
if (exist('t', 'var'))
    fclose(t);
    clear t;
end
t = tcpip('127.0.0.1', 20000, 'LocalPort', 20001);
t.ByteOrder = 'littleEndian';
t.InputBufferSize = 10000;
%t.DatagramReceivedFcn = 'ni_receivedData(t);';
t.BytesAvailableFcn = 'ni_receivedDataTCP(t, 4);';

fopen(t);


global res;
res =struct();

data = 1:6;

disp(' sending started...');
for i = 1:30
    data(1) = i;
    ni_sendData( t, data )
    pause(0.2);
end
disp(' done...');
disp(t);
  
fclose(t);

disp(res);
%https://github.com/kyamagu/matlab-tcpip