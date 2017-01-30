function AcqData = nexus_getdata(AcqSamplRateHz, AcqTimeSeconds, NumChannels, libnames, ForwardFetch)
% Function nexus_getdata( AcqSamplRateHz, AcqTimeSeconds, NumChannels)
%   - get acquired data.
% ------------------------------------------------------------------------
% 

if ~exist('ForwardFetch','var') || isempty(ForwardFetch)
    ForwardFetch = 0;
end

if exist('libnames','var') && ~isempty(libnames)
    libname = libnames{2};
else
    libnames = {'NexusAcqDLL', 'NexusGDIAcqDLL_x86', 'NexusGDIAcqDLL_x64'};
    for i = 1:length(libnames)
        if libisloaded(libnames{i})
            libname = libnames{i};
            break
        end
    end
end

    %libname = 'NexusAcqDLL';

    % Check arguments, use default arguments if needed
%     if nargin < 1
%        AcqSamplRateHz = 200; % default sampling rate (Hz)
%     end
%     if nargin < 2
%        AcqTimeSeconds = 1.0; % default acquisition time (s) - 1 s of data
%     end
%     
%     if nargin < 3
%         NumChannels = 64;
%     end
            
    numSamples = int32(AcqTimeSeconds * AcqSamplRateHz); % data buffer length (per each channel)
    
    % init data buffer memory
    dataBuf = nan(1,NumChannels*numSamples,'single');  % float in C

    % Get data
    if ForwardFetch
        [retval, dataBuf] = calllib(libname, 'NexusAPI_GetDataForward', numSamples, dataBuf);
    else
        [retval, dataBuf] = calllib(libname, 'NexusAPI_GetData', numSamples, dataBuf);
    end

    if retval ~= 0
        error(['Failed to get buffered data! ' num2str(retval) ]);
    end
        
    % De-scramble interleaved data (channel data are interleaved in data buffer)
   AcqData = zeros(NumChannels,numSamples,'single');
   for i = 1:NumChannels
       AcqData(i,:) = dataBuf(i:NumChannels:NumChannels*numSamples);
   end
% AcqData = dataBuf;
return;
    
