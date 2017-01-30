function [retval, libnames] = nexus_init( NumChannels , ForceLib )
% Function nexus_init_acq( NumChannels ) 
%   - initialize NeXus interface.
% ------------------------------------------------------------------------
%
    if strcmp(computer,'PCWIN64')
        uselib = 1;
    else
        uselib = 3;
    end
    
    if exist('ForceLib','var') && ~isempty(ForceLib)
        switch ForceLib
            case 1
                % Force 64-bit (GDI, 2011-12-06)
                uselib = 1;
                fprintf('Forced to use library type %i\n', uselib);
            case 2
                % Force 32-bit (GDI, 2011-12-06)
                uselib = 2;
                fprintf('Forced to use library type %i\n', uselib);
            case 3
                % Force 32-bit (Acq, 2010-01-26)
                uselib = 3;
                fprintf('Forced to use library type %i\n', uselib);
        end
    end

    if ~exist('NumChannels','var')
        NumChannels = 64;
    end

    % Load the library provided by MindMedia
    if uselib == 1 % GDI 64-bit (2011 version)
        dllPath = 'NexusGDIDLL_x64.dll';
        headerPath = 'NexusGDIDLL.h';
        libname = 'NexusGDIDLL_x64';
    elseif uselib == 2 % GDI 32-bit (2011 version)
        dllPath = 'NexusGDIDLL_x86.dll';
        headerPath = 'NexusGDIDLL.h';
        libname = 'NexusGDIDLL_x86';
    elseif uselib == 3 % GDI 32-bit (2009/2010 version)
        dllPath = 'NeXusDLL.dll';
        headerPath = 'NeXusDLL.h';
        libname = 'NeXusDLL';
    end
    if ~libisloaded(libname)
        loadlibrary(dllPath,headerPath);
        libnames{1} = libname;
    end

    % Load the library provided by Sergey
    if uselib == 1 % GDI 64-bit (2011 version)
        dllPath = 'NexusGDIAcqDLL_x64.dll';
        headerPath = 'NexusGDIAcqDLL.h';
        libname = 'NexusGDIAcqDLL_x64';
    elseif uselib == 2 % GDI 32-bit (2011 version)
        dllPath = 'NexusGDIAcqDLL_x86.dll';
        headerPath = 'NexusGDIAcqDLL.h';
        libname = 'NexusGDIAcqDLL_x86';
    elseif uselib == 3 % GDI 32-bit (2009/2010 version)
        dllPath = 'NexusAcqDLL.dll';
        headerPath = 'NexusAcqDLL.h';
        libname = 'NexusAcqDLL';
    end

    if ~exist(dllPath,'file')
        error('Nexus API DLL file does not exist');
    end

    if ~exist(headerPath,'file')
        error('Nexus API header file does not exist');
    end

    % Check if the library is already loaded
    if libisloaded(libname)
        unloadlibrary(libname);
    end

    % Turn off enum warnings
    warning off MATLAB:loadlibrary:enumexists;

    % Load the library
    loadlibrary(dllPath,headerPath);
    libnames{2} = libname;
    disp('Nexus API Library loaded OK');
    
    % Init
    retval = calllib(libname,'NexusAPI_Init', NumChannels);

    if retval ~= 0
        error('Failed to initialize Nexus device!');
    end

    disp(['Nexus device has been initialized for ' num2str(NumChannels) ' channels.']);

    return;
