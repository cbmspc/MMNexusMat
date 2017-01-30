function [dn, elapsedsec] = nexus_get_lastactive ()
Filename = ['.' filesep 'tmsi_api_stats.log'];
basedir = getbciprogramsdir();
a = dir(basedir);
dn = 0;
if exist(Filename, 'file')
    b = dir(Filename);
    if b.datenum > dn
        dn = b.datenum;
    end
end

for i = 1:length(a)
    if a(i).isdir && a(i).name(1) ~= '.'
        if exist([basedir filesep a(i).name filesep Filename], 'file')
            b = dir([basedir filesep a(i).name filesep Filename]);
            if b.datenum > dn
                dn = b.datenum;
            end
        end
    end
end

elapsedsec = (now - dn) * 86400;
