function [ ret ] = ni_receiveDataUDP( t )
%NI_RECEIVEDDATA Summary of this function goes here
%   Detailed explanation goes here

%res = struct('valid', false, 'id', 0, 'counter', 0, 'time', 0, 'nValues', 0, 'data', []);
global res;
data = fread(t, 1, 'uint8');

res(end+1).valid = false;
res(end).id = typecast(uint8(data(1:2)), 'uint16');
res(end).counter = typecast(uint8(data(3:6)), 'uint32');
res(end).time = typecast(uint8(data(7:14)), 'double');
res(end).nValues = typecast(uint8(data(15:16)), 'uint16');
nData = res(end).nValues;
for i = 1:nData
    res(end).data(i) = typecast(uint8(data( (17 +(i-1)*8):(24 +(i-1)*8) )), 'double');
end
id2 = typecast(uint8(data( (nData*8 +17) : (nData*8 +18)  )), 'uint16');

if (res(end).id == id2)
    res(end).valid = true;
end

ret = res(end);
end

