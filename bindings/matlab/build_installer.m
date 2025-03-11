%
% Copyright (c) 2024 Analog Devices Inc.
%
% This file is part of libm2k
% (see http://www.github.com/analogdevicesinc/libm2k).
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU Lesser General Public License as published by
% the Free Software Foundation, either version 2.1 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU Lesser General Public License for more details.
%
% You should have received a copy of the GNU Lesser General Public License
% along with this program. If not, see <http://www.gnu.org/licenses/>.
%

function build_installer()

version = '24.2.2';
ml = ver('MATLAB');
ml = ml.Release(2:end-1);

%%
cd(fileparts((mfilename('fullpath'))));
p = pwd;

fid  = fopen('bsp.tmpl','r');
f=fread(fid,'*char')';
fclose(fid);

% this affects system compatibility for the mltbx package 
win = 'false';
unix = 'false';
mac = 'false';

if isfile(fullfile(p, 'libm2k', 'libm2kInterface.dll'))
    win = 'true';
end

if isfile(fullfile(p, 'libm2k', 'libm2kInterface.so'))
    unix = 'true';
end

if isfile(fullfile(p, 'libm2k', 'libm2kInterface.dylib'))
    mac = 'true';
end


f = strrep(f,'__PROJECT_ROOT__',p);
f = strrep(f,'__REPO-ROOT__',p);
f = strrep(f,'__VERSION__',version);
f = strrep(f,'__ML-RELEASE__',ml);
f = strrep(f,'__LINUX__',unix);
f = strrep(f,'__WINDOWS__',win);
f = strrep(f,'__MAC_OS__',mac);

fid  = fopen('bsp.prj','w');
fprintf(fid,'%s',f);
fclose(fid);

addpath(genpath(matlabshared.supportpkg.getSupportPackageRoot));
addpath(genpath('.'));
rmpath(genpath('.'));
ps = {'doc','examples','libm2k'};
paths = '';
for p = ps
    pp = genpath(p{:});
    ppF = pp;
    pp = pp(1:end-1);
    pp = strrep(pp,':','</matlabPath><matlabPath>');
    paths = [paths,['<matlabPath>',pp,'</matlabPath>']]; %#ok<AGROW>
    addpath(ppF);
end
rehash
projectFile = 'bsp.prj';
currentVersion = matlab.addons.toolbox.toolboxVersion(projectFile);
outputFile = ['AnalogDeviceslibm2kBindings_v',currentVersion];
matlab.addons.toolbox.packageToolbox(projectFile,outputFile)

if ~usejava('desktop')
    %% Update toolbox paths
    mkdir other
    movefile([outputFile,'.mltbx'], ['other/',outputFile,'.zip']);
    cd other
    unzip([outputFile,'.zip'],'out');
    cd('out')
    cd('metadata');
    fid  = fopen('configuration.xml','r');
    f=fread(fid,'*char')';
    fclose(fid);
    
    s = '</matlabPaths>';
    sections = strsplit(f,s);
    s1 = sections{1};
    s2 = sections{2};
    newfile = [s1,paths,s,s2];
    
    fid  = fopen('configuration.xml','w');
    fprintf(fid,'%s',newfile);
    fclose(fid);
    
    %% Repack
    cd('..');
    zip([outputFile,'.zip'], '*');
    movefile([outputFile,'.zip'],['../../',outputFile,'.mltbx']);
    cd('../..');
    rmdir('other','s');
end

delete bsp.prj

%% Check output file
if isfile([outputFile,'.mltbx'])
    exit();
else
    exit(1);
end


