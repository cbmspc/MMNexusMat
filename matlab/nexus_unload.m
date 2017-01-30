function nexus_unload( )
% ------------------------------------------------------------------------
%
nexus_stop();

libnames = {'NexusAcqDLL', 'NexusGDIAcqDLL_x86', 'NexusGDIAcqDLL_x64', 'NexusGDIDLL_x86', 'NexusGDIDLL_x64'};

for i = 1:length(libnames)
    if libisloaded(libnames{i})
        unloadlibrary(libnames{i});
        disp('Nexus DLLs have been unloaded.');
        break
    end
end

