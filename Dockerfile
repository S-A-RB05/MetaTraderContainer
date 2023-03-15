# Use the base image from Ubuntu
FROM ubuntu:latest

# Set the working directory to /app
WORKDIR /app

# Install Wine, wget and MetaTrader 5 setup
RUN dpkg --add-architecture i386 \
    && apt-get update \
    && apt-get install -y wine32 winbind wget xvfb \
    && wget -q https://download.mql5.com/cdn/web/metaquotes.software.corp/mt5/mt5setup.exe
    #rm -rf /var/lib/apt/lists/*

# Set environment variable display to the host machine
#ENV DISPLAY=host.docker.internal:0.0

# Set up Xvfb
ENV DISPLAY=:99
RUN Xvfb :99 -screen 0 1024x768x16 &

COPY ["MetaTrader_5_32bit/MetaTrader 5", "./test"]