% For fusbi: Nchan = 34 (32 EEG, 1 Event, 1 Diagnostic)
% For synfi: Nchan = 67 (32 EEG, 1 Event, 32 EEG, 1 Event, 1 Diagnostic)
% SampleRate can be 128, 256, 512, 1024, 2048
% To access rawdata, mmfobj.Data.rawdata

function mmfobj = nexus_memmapfileobj (Nchan, SampleRate)
BufferDurationSec = floor(120*60*512/SampleRate);

SavePathName = gettmpdir();
mmfilename = [SavePathName filesep 'nexus_scope_mmf.dat'];
fid = fopen(mmfilename, 'w');
fwrite(fid, zeros(1,2), 'double');
fwrite(fid, zeros(1,1), 'double');
fwrite(fid, zeros(1,1), 'double');
fwrite(fid, zeros((Nchan+1)*BufferDurationSec*SampleRate,1), 'double');
fclose(fid);
mmfobj = memmapfile(mmfilename, 'Format', ...
    {
    'double', [1,2], 'dimensions'
    'double', [1,1], 'samplerate'
    'double', [1,1], 'recordendposition'
    'double', [Nchan+1, BufferDurationSec*SampleRate], 'rawdata'
    }, ...
    'Repeat', 1, 'Writable', true);
%buffersize = size(mmfobj.Data.rawdata,2);
mmfobj.Data.dimensions = [Nchan+1, BufferDurationSec*SampleRate];
mmfobj.Data.samplerate = SampleRate;
%rawdata = [];
%fprintf('Memory mapped file: %s \n',mmfilename);
