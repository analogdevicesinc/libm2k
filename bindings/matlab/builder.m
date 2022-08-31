%% Top level library builder
if isfile('definelibm2k.m')
    delete definelibm2k.m
end

% Generate XML data cache
if ispc
    build_library_win64
    delete definelibm2k.m
    copyfile('definelibm2k_win64.m','definelibm2k.m');
else
    build_library_linux64
    delete definelibm2k.m
    copyfile('definelibm2k_linux64.m','definelibm2k.m');
end

% Build library
try
    pkg = definelibm2k;
    build(pkg);
catch
end

if ispc
    f = isfile(fullfile('libm2k','libm2kInterface.dll'));
else
    f = isfile(fullfile('libm2k','libm2kInterface.so'));
end

exit(~f);