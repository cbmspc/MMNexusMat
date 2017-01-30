% Automatically prompt to retry Nexus
retry = 1;
while retry
    try
        disp('Load test');
        nexus_init(33);
        disp('UnLoad test');
        nexus_unload();
        retry = 0;
    catch
        fprintf('\n\n');
        fprintf('Error initializing Nexus. Check:\n');
        fprintf('* Turn on the amplifiers\n');
        fprintf('* Plug in optical fiber cables\n');
        fprintf('* Plug in SynFi/FUSBI to computer via USB\n');
        fprintf('\n\n');
        q = questdlg(['Cannot initialize Nexus EEG amplifier. Check amplifier power, optical fiber connection, and USB connection.' 10 10 'Retry?'], 'nexus_init error', 'Yes', 'No', 'Yes');
        if strcmpi(q,'Yes')
            retry = 1;
        else
            retry = 0;
            error('nexus_init error');
        end
    end
end