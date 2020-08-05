function simulink_NetworkInterface_set_signal_parameter_names(gcb, init_command, offsetInput, offsetOutput, sizeInput, sizeOutput)

%% Mask command
command = init_command;

%% Get importand handles
handleNetworkInterface = get_param( gcb,'handle');
portsNetworkInterface  = get_param(handleNetworkInterface, 'PortHandles');

%% Set input names
if (sizeInput > 0)
    for i = (1+offsetInput):length(portsNetworkInterface.Inport)
        %inputDimension = varInputs(i-inputOffset);
        portHandle = portsNetworkInterface.Inport(i);
        port = get(portHandle);
        
        line_name = ['IN ', num2str(i -offsetInput)];
        lineHandle = port.Line;
        if ishandle(lineHandle)
            line = get(lineHandle);
            if length(line.Name)
                line_name = line.Name;
            end
        end
        
        command = [command, char(10), ['port_label(''input'', ' num2str(i) ', ''' line_name ''');']];
    end
end


%% Set output names
if (sizeOutput > 0)
    for i = (1+offsetOutput):(length(portsNetworkInterface.Outport)-1)
        portHandle = portsNetworkInterface.Outport(i);
        port = get(portHandle);
        
        line_name = ['OUT ', num2str(i -offsetOutput)];
        lineHandle = port.Line;
        if ishandle(lineHandle)
            line = get(lineHandle);
            if length(line.Name)
                line_name = line.Name;
            end
        end
        
        command = [command, char(10), ['port_label(''output'', ' num2str(i) ', ''' line_name ''');']];
    end
    command = [command, char(10), ['port_label(''output'', ' num2str(i+1) ', ''MsgCounter'');']];
end

set_param(gcb,'MaskDisplay',command);
end
