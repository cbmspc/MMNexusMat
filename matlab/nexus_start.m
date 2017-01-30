function retval = nexus_start(samplingRate, BufferSizeSeconds)
% Function nexus_start() - start data acquisition.
% ------------------------------------------------------------------------
% Starts data acquisition on NeXus device.
% The function returns 'OK' on success or raises error on failure.
%

libnames = {'NexusAcqDLL', 'NexusGDIAcqDLL_x86', 'NexusGDIAcqDLL_x64', 'NexusGDIDLL_x86', 'NexusGDIDLL_x64'};
for i = 1:length(libnames)
    if libisloaded(libnames{i})
        libname = libnames{i};
        break
    end
end

%libname = 'NexusAcqDLL';
    
    % Start acquisition
    retval = calllib(libname, 'NexusAPI_Start', samplingRate, BufferSizeSeconds);

    if retval ~= 0
        error('Failed to start data acquisition!'); 
    end
    
    disp(['Nexus device has started data acquisition at ' num2str(samplingRate) ' Hz sample rate and ' num2str(BufferSizeSeconds) ' s buffer.']);
    
    return;
    
