function [] = publishmatbiipsexamples(varargin)
% Create html files for matbiips examples, and matlab zip files
% Last update: 26/08/2014

outdir = 'D:/caron/Dropbox/biips/website/examples/';
% outdir = '/home/adrien-alea/Dropbox/Biips/biips-share/website/examples/';
% outdir = 'C:/Users/adrien/Dropbox/Biips/biips-share/website/examples/';
if nargin>=1
    outdir = varargin{1};
end

ind_folders = 1:4;
if nargin>=2
    ind_folders = varargin{2};
end

options = struct();
if nargin>=3
    options = varargin{3};
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
    {'stoch_kinetic_gill', 'stoch_kinetic'},...
    {'switch_stoch_volatility', 'stoch_volatility', 'switch_stoch_volatility_param'}...
    };

fprintf('========================================================\n')
for i=ind_folders
    mdir = fullfile('.', name_folders{i});
    cd(mdir);
    options.outputDir = fullfile(outdir, name_folders{i}, 'matbiips');
    files_i = names_mfiles{i};
    for j=1:length(files_i)
        % Publish html file
        fprintf('Publishing matbiips example: %s\n', files_i{j})
        publish([files_i{j} '.m'], options);
        close all
        fprintf('------------------------------------------------\n')         
    end
    
    cd('../')    
end
