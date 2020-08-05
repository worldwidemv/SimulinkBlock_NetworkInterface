%% Install SRT Blocks - NetworkInterface

%   TU Berlin --- Fachgebiet Regelungssystem
%   Author: Markus Valtin
%   Copyright © 2020 Markus Valtin. All rights reserved.
%
%   This program is free software: you can redistribute it and/or modify it under the terms of the 
%   GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.
%
%   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
%   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

%% check if the neccesarry packages are installed
if (~exist('.installDone', 'file'))
    uiwait(msgbox({'Please install the following system library packages,', ...
            'using the systems package manager:', ...
            ' * libboost_system', ...
            ' * libboost_thread', ...
            ' * libboost_signals', ...
            '', ...
            '      Proccede after the packages are installed.'}, ...
            'NetworkInterface Dependencies', 'modal'));
            
end

%% mark this block Lib as installed
fileID = fopen('.installDone','w');
fwrite(fileID,' ');
fclose(fileID);