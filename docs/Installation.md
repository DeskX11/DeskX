
## Intro
Here is a step-by-step instruction for installing the program on a remote server. The case will be considered when you have access to the server via SSH, which is running on Ubuntu. We will use xfce4 as the desktop environment.

## Step 1: Downloading the necessary components
```
sudo apt update && sudo apt upgrade -y
sudo apt install xfce4 g++ make libx11-dev libxtst-dev libxext-dev git
git clone https://github.com/DeskX11/DeskX
```
Here we have downloaded xfce4, the C++ language compiler and the DeskX program itself. An alternative way to install DeskX on Debian-like systems is to download the prebuilt `.deb` package from <a href="https://github.com/DeskX11/DeskX/releases">here</a>.

## Step 2: X11 setup
For X11 to work properly (given that this is a remote server that does not have a monitor), we need to emit the presence of a monitor, this can be done using a special driver `xserver-xorg-video-dummy`.
```
sudo apt install xserver-xorg-video-dummy
```
The next step is to add a monitor to X11 and specify the screen resolution you need to work.
```
sudo nano /etc/X11/xorg.conf
```
Next, add the code below to the file and save the result
```
Section "Device"
    Identifier  "Configured Video Device"
    Driver "dummy"
    VideoRam 6230000
EndSection

Section "Monitor"
    Identifier  "Configured Monitor"
    HorizSync 31.5-48.5
    VertRefresh 50-70
    Modeline "1600x900" 33.92 1600 1632 1760 1792 900 921 924 946
EndSection

Section "Screen"
    Identifier  "Default Screen"
    Monitor     "Configured Monitor"
    Device      "Configured Video Device"
    DefaultDepth 24
    SubSection "Display"
        Depth 24
        Virtual 1600 900
    EndSubSection
EndSection

```

## Step 3: Launch
Now it remains to compile the program and run everything.
```
cd DeskX
make server

sudo startxfce4 &
sudo ./dxs --port=6532 --display=:0 --xauth=../.Xauthority
```
The .Xauthority file is usually located in the user's home directory. You can already change the connection port (as well as other parameters) for yourself.