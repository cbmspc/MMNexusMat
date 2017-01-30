% Dialog to select the montage

% Master channels list: get_eeg_sensor_montage.m
%
%

function [selchanset, ChanNames, Montage, NexusNumber] = nexus_prompt_select_montage (origin)
if ~exist('origin','var')
    origin = '';
end

[Montage, MontageDesc, NexusNumS] = get_eeg_sensor_montage();
fn = fieldnames(Montage);
strlist = cell(length(fn)+2,1);
strlist{1} = '--Select Config--';
strlist{end} = '';

for i = 1:length(fn)
    strlist{i+1} = sprintf('%s (%s)', fn{i}, MontageDesc.(fn{i}));
end

strlist(1:end-1) = sort(strlist(1:end-1));

if ~strcmpi(origin, 'nexuschannels')
    strlist{end} = 'userdefined (Enter manually)';
else
    strlist = strlist(1:end-1);
end

q = listdlg('ListString', strlist, 'SelectionMode', 'single', 'PromptString', 'Nexus channel arrangement', 'InitialValue', 1, 'ListSize', [240 250]);
selchanset = regexp(strlist{q}, '^\w+', 'match', 'once');
if ~isempty(selchanset)
    selchanset = lower(selchanset);
end
if isempty(selchanset)
    ChanNames = {};
    NexusNumber = [];
elseif strcmp(selchanset, 'userdefined')
    assignin('base', 'tmp_NexusChanNames', []);
    uiwait(NexusChannels);
    pause(0.1);
    tmp_NexusChanNames = evalin('base', 'tmp_NexusChanNames');
    ChanNames = tmp_NexusChanNames.ChanNames;
    NexusNumber = NexusNumS.(selchanset);
else
    Montage = get_eeg_sensor_montage();
    ChanNames = Montage.(selchanset);
    NexusNumber = NexusNumS.(selchanset);
end
