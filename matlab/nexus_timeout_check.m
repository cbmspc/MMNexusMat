
[~, tmp] = nexus_get_lastactive();
if tmp > 298
    q = questdlg(['Please turn on the amplifiers.' 10 10 'Continue?'], 'nexus_init timeout?', 'Continue', 'Abort', 'Continue');
    if strcmpi(q,'Continue')
    else
        error('Procedure aborted by operator.');
    end
end


% if ~exist('naa_tnexusactive','var') || isempty(naa_tnexusactive) || toc(naa_tnexusactive) > 290
%     q = questdlg(['It has been over 5 minutes from the last session, and the amplifiers might have powered off.' 10 10 'Continue?'], 'nexus_init timeout?', 'Continue', 'Abort', 'Continue');
%     if strcmpi(q,'Continue')
%     else
%         error('Procedure aborted by operator.');
%     end
% end
