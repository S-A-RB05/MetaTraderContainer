# Instructions for running Dockers

## MetaTrader 5 with GUI for testing
To build docker file: ```docker build -t ubuntu-mt5-gui "USE DOCKER FILE MT5-GUI"```  
To run docker file: ```docker run -d -v "ABSOLUTE WINDOWS PATH TO REPORT DIRECTORY":/MetaTrader/Report ubuntu-mt5-gui```  
To run MetaTrader 5: ```wine terminal.exe```

## MetaTrader 5 without GUI for production
To build docker file: ```docker build -t ubuntu-mt5-no-gui "USE DOCKER FILE MT5-NO-GUI"```  
To run docker file: ```docker run -d -v "ABSOLUTE WINDOWS PATH TO REPORT DIRECTORY":/MetaTrader/Report ubuntu-mt5-no-gui``` 
To run MetaTrader 5: ```xvfb-run wine terminal.exe```
