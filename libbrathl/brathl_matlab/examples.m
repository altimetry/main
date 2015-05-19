% BRATHL MATLAB interface examples

% Date diff example:

d1.year=2009;
d1.month=9;
d1.day=3;
d1.hour=12;
d1.minute=25;
d1.second=5;
d1.muSecond=0;

d2.year=2009;
d2.month=9;
d2.day=3;
d2.hour=12;
d2.minute=25;
d2.second=15;
d2.muSecond=0;

% Output should be: 10
diff = brathl_DiffYMDHMSM(d2, d1)

% -----------------------------------------------------------------

% Product file ingestion example. 
% NOTE: This example does not yet work in BRAT 2.0.

% Set data input file
files={'E:\brat\data_sample\jason1\JA1_GDR_2PaP124_001.CNES';
       'E:\brat\data_sample\jason1\JA1_GDR_2PaP124_002.CNES';
       'E:\brat\data_sample\jason1\JA1_GDR_2PaP124_003.CNES'};

% Set record name 
record='data';

% Set data selection - (set selection = "" to retrieve all data row)
selection='latitude > 20 && latitude < 30';

% Set expressions (here 2 expressions) 
expr='latitude + longitude';

% Set units for each expression
units='radians';

ignoreOutOfrange=false;

% No statistics
statistics=false;

% Default value is 0
defaultValue=0;

% Call ReadData function
dataResults = brathl_ReadData(files, record, selection, expr, units, ignoreOutOfrange, statistics, defaultValue)

%-----------------------------------------------------------------
