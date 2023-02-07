function build_installer()

version = '22.2.1';
ml = ver('MATLAB');
ml = ml.Release(2:end-1);
arch = computer('arch');

%%
cd(fileparts((mfilename('fullpath'))));
p = pwd;

fid  = fopen('bsp.tmpl','r');
f=fread(fid,'*char')';
fclose(fid);

if ispc
    win = 'true';
    unix = 'false';
else
    win = 'false';
    unix = 'true';
end
f = strrep(f,'__REPO-ROOT__',p);
f = strrep(f,'__VERSION__',version);
f = strrep(f,'__ML-RELEASE__',ml);
f = strrep(f,'__ARCH__',arch);
f = strrep(f,'__LINUX__',unix);
f = strrep(f,'__WINDOWS__',win);

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


