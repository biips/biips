function [] = publishmatbiipsexamples(varargin)
% Create html files for matbiips examples, and matlab zip files
% Last update: 26/08/2014

ind_folders = 1:4;
if nargin>=1
    ind_folders = varargin{1};
end

outdir = 'D:/caron/Dropbox/biips/website/examples/';
% outdir = '/home/adrien-alea/Dropbox/Biips/biips-share/website/examples/';
if nargin>=2
    outdir = varargin{2};
end

name_folders = {...
    'tutorial',...
    'object_tracking',...
    'stoch_kinetic',...
    'stoch_volatility'...
    };
names_mfiles = {...
    {'tutorial1', 'tutorial2', 'tutorial3'},...
    {'hmm_4d_nonlin'},...
    {'stoch_kinetic', 'stoch_kinetic_gill'},...
    {'stoch_volatility', 'switch_stoch_volatility', 'switch_stoch_volatility_param'}...
    };
names_addfiles = {...
    {{'hmm_1d_nonlin.bug'}, {'hmm_1d_nonlin_param.bug'}, {'hmm_1d_nonlin_funmat.bug', 'f_dim.m', 'f_eval.m'}},...
    {{'hmm_4d_nonlin_tracking.bug'}},...
    {{'stoch_kinetic.bug'}, {'stoch_kinetic_gill.bug', 'lotka_volterra_gillepsie', 'lotka_volterra_dim'}},...
    {{'stoch_volatility.bug', 'SP500.csv'}, {'switch_stoch_volatility.bug', 'SP500.csv'}, {'switch_stoch_volatility_param.bug', 'SP500.csv'}}...
    };
nfolders = length(name_folders);

for i=ind_folders
    mdir = ['./', name_folders{i} '/'];
    cd(mdir);
    outputdir = [outdir  name_folders{i} '/matbiips/'];
    files_i = names_mfiles{i};
    for j=1:length(files_i)
        % Publish html file
        publish([files_i{j} '.m'], 'format', 'html', 'outputDir', outputdir);
        close all
        
%         addfiles = names_addfiles{i}{j};
%         addfiles{end+1} = [files_i{j}, '.m'];
%         % zip the Matlab and bugs files
%         zip(files_i{j}, addfiles);
%         movefile([files_i{j}, '.zip'], [outputdir, files_i{j}, '.zip']);            
    end
    
    cd('../')    
end
