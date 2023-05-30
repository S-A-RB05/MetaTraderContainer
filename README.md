# Releases

## V1.3
rabbitmq implementation it converts report t csv and sends it to queue back with jobid and csv as string with /n seperators.


## V1.2
Docker has improved way of testing through the use of a script, this means the docker will start grab a basic config file and start the test with the config file, writes the report to the Report folder and closes MT5. This makes it able to be used as a kubernetes job and doesn't require the ```-it``` flag to enter the docker's terminal.  
To start docker ```docker run -it -v "ABSOLUTE WINDOWS PATH TO REPORT DIRECTORY":/MetaTrader/Report ubuntu-mt5-no-gui```

The -v flag will mount the volume you want to receive the reports from. You can change [the script](https://github.com/S-A-RB05/MetaTraderContainer/blob/main/MetaTrader_5_32bit/MetaTrader5/run_test_nogui.sh) to grab the correct config.

This release is implemented for the gui as well as the no gui version.




## V1.1
The same docker as V1.0 but without GUI and need for a display, because it creates a virual one.

### Instructions MetaTrader 5 without GUI for production
To build docker file: ```docker build -t ubuntu-mt5-no-gui "USE DOCKER FILE MT5-NO-GUI"```  
To run docker file: ```docker run -it -v "ABSOLUTE WINDOWS PATH TO REPORT DIRECTORY":/MetaTrader/Report ubuntu-mt5-no-gui``` 
To run MetaTrader 5: ```xvfb-run wine terminal.exe```
To test a strategy with MetaTrader 5: ```wine terminal.exe /config:"Config\file_you_want_to_use.ini```


## V1.0
Basic docker with MetaTrader 5 install directory that can launch MT5 and perform tests with commands through the docker terminal. This version needs an X-server installed so the docker can use the display from the host to launch MT5.

### Instructions MetaTrader 5 with GUI for testing
To build docker file: ```docker build -t ubuntu-mt5-gui "USE DOCKER FILE MT5-GUI"```  
To run docker file: ```docker run -it -v "ABSOLUTE WINDOWS PATH TO REPORT DIRECTORY":/MetaTrader/Report ubuntu-mt5-gui```  
To run MetaTrader 5: ```wine terminal.exe```
To test a strategy with MetaTrader 5: ```wine terminal.exe /config:"Config\file_you_want_to_use.ini```





