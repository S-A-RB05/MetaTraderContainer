# Use the base image from Ubuntu
FROM ubuntu:latest

# Set the working directory to /app
WORKDIR /MetaTrader

# Tells docker that during installations it is not possible for the GUI to interact
ENV DEBIAN_FRONTEND=noninteractive

# Install Wine, wget and MetaTrader 5 setup
RUN dpkg --add-architecture i386 \
    && apt-get update \
    && apt-get install -y wine32 winbind wget xvfb

# Set up Xvfb
ENV DISPLAY=:99
RUN Xvfb :99 -screen 0 1024x768x16 &

COPY ["MetaTrader_5_32bit/MetaTrader5", "./"]