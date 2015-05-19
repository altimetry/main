function varargout = brathl_ReadData(varargin)

[varargout{1:max(1,nargout)}] = brathl_matlab('READDATA',varargin{:});
