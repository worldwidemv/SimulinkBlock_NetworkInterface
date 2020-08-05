function simulink_NetworkInterface_init( gcb, varInputs, varOutputs)
    maxAmountOfSignals = 200;
    load_system(gcb);

    sizeInputs  = sum(varInputs);
    sizeOutputs = sum(varOutputs);

    amountIn =size(varInputs ,2);
    if amountIn < 0, amountIn = 0; end
    amountOut=size(varOutputs,2);
    if amountOut < 0, amountOut = 0; end

    %% Handles
    handle_LowLevelBlock = get_param([gcb '/LowLevel NetInterface_01'],'handle');
    handle_NetDelayBlock = get_param([gcb '/NetDelay'],'handle');

    % Adjust input mux settings
    muxInput = find_system(gcb,...
          'LookUnderMasks','on',...
          'FollowLinks','on','Name','InputMux');
    if isempty(muxInput)
       warning('NetworInterface: Input mux not found!')
       return;
    end
    
    handleInputMux = get_param([gcb '/InputMux'],'handle');
    
    %% Input
    % Remove current lines at input mux
    simulink_NetworkInterface_remove_block(handleInputMux, 1)
    simulink_NetworkInterface_connect_two_blocks(gcb, handleInputMux, 1, handle_LowLevelBlock, 2)
    
    % Adjust needed amount of ports
    set_param(handleInputMux,'Inputs',num2str(amountIn));
    
    if (sizeInputs == 0)
        %% Input is EMPTY
        handleResetInput = get_param([gcb '/Reset'],'handle');
        simulink_NetworkInterface_connect_two_blocks(gcb, handleResetInput, 1, handleInputMux, 1);
        
        % Delete non needed inputs
        for i=1:maxAmountOfSignals
            inputBlock = ['In' num2str(i)];
            block = find_system(gcb,'LookUnderMasks','on', 'FollowLinks','on','Name',inputBlock);
            if ~isempty(block)
                handleBlock = get_param([gcb '/' inputBlock],'handle');
                simulink_NetworkInterface_remove_block(handleBlock,0)
            else
                break;
            end
        end
    else
        %% Input is NOT EMPTY
        % Create missing inputs
        for i=1:amountIn
            inputBlock = ['In' num2str(i)];
            block = find_system(gcb,...
                'LookUnderMasks','on',...
                'FollowLinks','on','Name',inputBlock);
            if isempty(block)
                add_block('built-in/Inport',[gcb, ['/' inputBlock]]);
            end
            set_param([gcb '/' inputBlock],'PortDimensions', num2str(varInputs(i)));
            
            %Connect input with signal spect
            handleInputSrc = get_param([gcb '/' inputBlock],'handle');
            simulink_NetworkInterface_connect_two_blocks(gcb, handleInputSrc, 1, handleInputMux, i)
        end
        
        % Delete non needed inputs
        for i=(amountIn+1):maxAmountOfSignals
            inputBlock = ['In' num2str(i)];
            block = find_system(gcb,'LookUnderMasks','on','FollowLinks','on','Name',inputBlock);
            if ~isempty(block)
                handleBlock = get_param([gcb '/' inputBlock],'handle');
                simulink_NetworkInterface_remove_block(handleBlock,0);
            else
                break;
            end
        end
    end
    
    
    %% OUTPUT
    if (sizeOutputs == 0)
        %% Output is EMPTY
        % delete the MsgCounter output
        blockMsgCounter = find_system(gcb, 'LookUnderMasks','on','FollowLinks','on','Name','MsgCounter');
        if ~isempty(blockMsgCounter)
            simulink_NetworkInterface_remove_block([gcb, '/MsgCounter']);
            add_block('built-in/Terminator',[gcb, '/TermMsgCounter']);
            simulink_NetworkInterface_connect_two_blocks(gcb, handle_LowLevelBlock, 2, [gcb, '/TermMsgCounter'], 1);
        end
        
        % Delete non needed outputs
        for i=1:maxAmountOfSignals
            outputBlock = ['Out' num2str(i)];
            selectorBlock = ['Selector' num2str(i)];
            block = find_system(gcb,'LookUnderMasks','on','FollowLinks','on','Name',outputBlock);
            if ~isempty(block)
                handleSelector = get_param([gcb '/' selectorBlock],'handle');
                simulink_NetworkInterface_remove_block(handleSelector)
                delete_block(block);
            else
                break;
            end
        end
        if ~isempty(blockMsgCounter)
            add_block('built-in/Terminator',[gcb, '/TermOut']);
            simulink_NetworkInterface_connect_two_blocks(gcb, handle_NetDelayBlock, 1, [gcb, '/TermOut'], 1);
        end
    else
        %% Output is NOT EMPTY
        % delete Terminators
        blockTermMsgCounter = find_system(gcb, 'LookUnderMasks','on','FollowLinks','on','Name','TermMsgCounter');
        if ~isempty(blockTermMsgCounter)
            simulink_NetworkInterface_remove_block([gcb, '/TermMsgCounter']);
            simulink_NetworkInterface_remove_block([gcb, '/TermOut']);
        end
        
        % Update the MsgCounter output
        blockMsgCounter = find_system(gcb, 'LookUnderMasks','on','FollowLinks','on','Name','MsgCounter');
        if isempty(blockMsgCounter)
            % msgCounter output is not there
            add_block('built-in/Outport',[gcb, '/MsgCounter']);
            simulink_NetworkInterface_connect_two_blocks(gcb, handle_LowLevelBlock, 2, [gcb, '/MsgCounter'], 1);
        end
        blockMsgCounter = [gcb '/MsgCounter'];
        set_param(blockMsgCounter, 'Port', num2str(amountOut+1));
        
        % Create missing outputs
        for i=1:amountOut
            outputBlock = ['Out' num2str(i)];
            selectorBlock = ['Selector' num2str(i)];
            
            block = find_system(gcb,'LookUnderMasks','on','FollowLinks','on','Name',outputBlock);
            if isempty(block)
                add_block('built-in/Outport',[gcb, ['/' outputBlock]]);
            end
            
            % Create selector per outport
            block = find_system(gcb,'LookUnderMasks','on','FollowLinks','on','Name',selectorBlock);
            if isempty(block)
                add_block('built-in/Selector',[gcb, ['/' selectorBlock]]);
            end
            
            handleSelect  = get_param([gcb '/' selectorBlock],'handle');
            handleOutport = get_param([gcb '/' outputBlock]  ,'handle');
            
            start_i = sum(varOutputs(1:i-1)) + 1;
            end_i   = start_i + varOutputs(i) - 1;
            range = ['[' num2str(start_i) ':' num2str(end_i) ']'];
            set_param(handleSelect, 'IndexParamArray', {range},'InputPortWidth', num2str(sizeOutputs));
            
            % Create connection selector <-> output
            simulink_NetworkInterface_connect_two_blocks(gcb, handleSelect, 1, handleOutport, 1)
            
            % Create connection LowLevel NetworkInterface <-> selector
            simulink_NetworkInterface_connect_two_blocks(gcb, handle_NetDelayBlock, 1, handleSelect, 1)
        end
        
        % Delete non needed outputs
        for i=amountOut+1:maxAmountOfSignals
            outputBlock = ['Out' num2str(i)];
            selectorBlock = ['Selector' num2str(i)];
            
            block = find_system(gcb,'LookUnderMasks','on','FollowLinks','on','Name',outputBlock);
            if ~isempty(block)
                handleSelector = get_param([gcb '/' selectorBlock],'handle');
                simulink_NetworkInterface_remove_block(handleSelector)
                delete_block(block);
            else
                break;
            end
        end
        
    end
    % Create connection LowLevel NetworkInterface <-> delay block
    %simulink_NetworkInterface_connect_two_blocks(gcb, handle_NetDelayBlock, 1, handleSelect, 1);
    
    % Update the MsgCounter output
    %set_param(blockMsgCounter, 'Port', num2str(amountOut+1));
    
    
    %% update the signal names
    initCommand = ['%% lables', char(10), ...
        'color(''blue''); text(0.5, 0.5, ''NetworkInterface'',''horizontalAlignment'',''center'');', ...
        char(10), char(10), 'color(''black'');', char(10), 'port_label(''input'',  1  ,'' Control '');', char(10)];
    simulink_NetworkInterface_set_signal_parameter_names(gcb, initCommand , 1, 0, sizeInputs, sizeOutputs);
end
