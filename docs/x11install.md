
## Intro
The process of preparing a server for remote control will be described below. All actions will be performed on the Ubuntu distribution via ssh.

## Step 1: Downloading the necessary components
```bash
sudo apt update && sudo apt upgrade -y
sudo apt install xfce4 g++ make libx11-dev libxtst-dev libxext-dev libportal-dev libpipewire-0.3-dev git libsdl2-dev
git clone https://github.com/DeskX11/DeskX
```
Here we download the window manager ABAB and the necessary components for compiling DeskX. You can skip downloading them and just install the ready-made deb package from [here](https://github.com/DeskX11/DeskX/releases).

## Step 2: X11 setup
For X11 to work properly (given that this is a remote server that does not have a monitor), we need to emit the presence of a monitor, this can be done using a special driver `xserver-xorg-video-dummy`.
```bash
sudo apt install xserver-xorg-video-dummy
```
The next step is to add a monitor to X11 and specify the screen resolution you need to work.
```bash
sudo nano /etc/X11/xorg.conf
```
Next, add the code below to the file and save the result
```bash
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
## Step 3: Compiling and running VM
Now it remains to compile the program and run WM. 
```bash
cd DeskX
make
sudo startxfce4 &
```
Now everything is ready to launch DeskX. All that remains is to create the necessary environment, you can read about it [here](https://github.com/DeskX11/DeskX/blob/main/docs/howtouse.md).