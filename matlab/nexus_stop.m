function retval = nexus_stop()
% Function nexus_close() - stop data acquisition.
% ------------------------------------------------------------------------
%
    libnames = {'NexusAcqDLL', 'NexusGDIAcqDLL_x86', 'NexusGDIAcqDLL_x64'};
    
    for i = 1:length(libnames)
        if libisloaded(libnames{i})
            retval = calllib(libnames{i}, 'NexusAPI_Stop');
            if retval ~= 0
                error('Failed to stop data acquisition!');
            end
            disp('Nexus device has stopped data acquisition.');
            break
        end
    end
       
    
    return;
    