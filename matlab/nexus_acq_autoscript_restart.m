% Restart script for nexus_acq_autoscript
cwd = pwd;
tempdir = getenv('tmp');
if ~exist('TrainDB','var')
    TrainDB = [];
end
if ~exist('SJindatabase','var')
    SJindatabase = [];
end
if ~exist('naa_in4','var')
    naa_in4 = [];
end

UpdateStatus('MATLAB restarting');

save([tempdir '\Parameters-autosave.mat'], 'ChanNames', 'SWcar', 'naa_tstart', 'NexusAcqSampleRate', 'SWfilter', 'naa_in1', 'NexusNum', 'SWplot', 'naa_in2', 'SJindatabase', 'SessionNum', 'naa_in3', 'SJprivacy', 'SubjectName', 'naa_in4', 'retry', 'cwd', 'TrainDB', 'NumRecordedSessions', 'LastRecordedSession');
fprintf('MATLAB will auto-restart: ...');
for js = 11:-1:1
    fprintf('\b\b\b%3i', js);
    pause(1.0);
end
clc
try
    a = pathdef;
    b = regexp(a, '([^;]+(?i)MATLAB(?-i)[^;]*[\\/]toolbox[^;]+);', 'tokens', 'once');
    c = regexp(b{1}, '^(.+)\\toolbox', 'tokens', 'once');
    m = [c{1} '\bin\matlab.exe'];
    if exist(m, 'file')
        eval(['!' '"' m '" -nosplash']);
    end
catch
    fprintf('Auto-restart failed. MATLAB is exiting.\n');
    pause(5.0);
end
quit('force');
