<p align="center"><img height="120px" src="./info/logo.png"></p>
<h4 align="center">DeskX - Remote control program</h4>
<p align="center"><a align="center" href="https://github.com/DeskX11/DeskX/actions/workflows/build.yml"><img align="center" src="https://github.com/DeskX11/DeskX/actions/workflows/build.yml/badge.svg"></a></p>

## About

The project was created for its own use within the home local network (you can use DeskX over the internet using port forwarding via ssh, with such a connection you only need to use the TCP protocol). I have a server at home with a large number of Linux virtual machines to which I needed remote access. All remote software I've tried had their problems - e.g., scrolling in the browser, dragging windows, etc. For this reason, a project was created that focuses on the fastest possible data transfer over the local network. The project works on all operating systems with X11.

[You can leave your opinion - I am happy to get any feedback ;)](https://github.com/DeskX11/DeskX/discussions/4)

## Functionality

* Selection of the range of the difference between adjacent pixels to ensure compression (from 0 to 255)
* Multiple Commands: Server Side Shutdown, Remote Control
* Ability to create a palette of colors
* Ability to set an authorization password
* There is a choice of data transfer protocol (TCP or UDP)
* Fullscreen or windowed mode
* Two separate streams for transferring screen data and events

## An example of working with compression

<p align="center"><img src="./info/example.png"></p>
<a href="https://youtu.be/pDRSAVssPek">Video example<a> of how the program works (compression parameter is 16, protocol for events and screen is TCP, gigabit LAN is used)

## Compression algorithm

The first step is to generate a hash table of colors that are most often found on the screen (up to 256 colors) on the server side. The color table is sent to the client side. Subsequently, 1 byte of the color identifier can be written to the place of writing 3 bytes of color. The next step is the lossy compression of the frame based on the allowable difference between the color bytes (the range of the difference can be changed by the user, this affects the packet size and picture quality). Also, in the process of sending the next frames, the program detects unchanged areas and makes an indication that nothing needs to be changed in this place on the screen, if it fails, it tries to refer to a similar color vertically or horizontally. Thus, it is possible to significantly reduce the size of the transmitted packet.

## Screen resolution

Controlling screen resolution and picture sizes is entirely dependent on the server-side resolution settings. To change it, use the standard utility xrandr.

## Window or Fullscreen mode

Fullscreen mode is enabled when your screen resolution matches the server side screen resolution. Press the `F7` key to exit fullscreen mode.

## Getting started
* Build it
```bash
# Debian/Ubuntu
sudo apt install libx11-dev libxtst-dev libssl-dev libxext-dev
git clone https://github.com/DeskX11/DeskX/
cd DeskX
make client
make server
# After these steps 2 files will be compiled: `dxc` (the client part, which must be launched on the
# computer from which the control will be carried out) and `dxs` (server part for a managed computer).
```

* Build the .deb packages
```bash
# Debian/Ubuntu
sudo apt install libx11-dev libxtst-dev libssl-dev libxext-dev
git clone https://github.com/DeskX11/DeskX/
cd DeskX
make deb-client
make deb-server
# After that, you will have the client and server .deb packages in the project's root folder.
```

* Or you can download packages from the <a href="https://github.com/DeskX11/DeskX/releases">Release section</a>

* Also, there is an <a href="https://aur.archlinux.org/packages/deskx-git/">AUR repository</a> for Arch Linux users (Thanks to joserebelo!)

## Usage
### Client
```
Usage: ./dxс [options]
Options:
	--ip			Ip address of the server
	--port			Port of the server
	--password		Verification secret word without spaces
	--compression		Compression range (0 to 255)
	--events		Protocol for events, TCP or UDP (default: TCP)
	--screen		Protocol for screen, TCP or UDP (default: TCP)
	--disable-vert		Disable vertical compression.
	--cmd			Server side command (default: rat)

Commands:
	exit			Command to shutdown the server side
	rat 			Start remote control
	screenshot		Get a picture of the server desktop

Example:
	./dxс --ip=192.168.0.1 --port=4431 --password=secret --compression=16
```
### Server
```
Usage: ./dxs [options]
Options:
	--port			Connection port
	--password		Verification secret word without spaces
	--display		Screen number (:0)
	--xauth			Path to .Xauthority file
	--palette		Path to palette file (default: ./palette.deskx)
	--cmd			Command (default: rat)

Commands:
	palette			Palette Generation Mode
	rat 			Start remote control
	all			Launching the two previous modes at once

Example:
	./dxs --cmd=all --port=4431 --password=secret
```

## How to get best performance?

Fast data transfer depends on two factors: the size of the transmitted packet and the transfer rate. To reduce the size of the batch, you can try changing the `--compression` parameter up. It also helps to reduce the size of the package by creating a color picker that captures the desktop background and interface elements. To create a palette on the server side, you need to run dxs with `--cmd=palette`. To reduce transmission latency, you can try changing the data transfer protocol for both screen and events (according to my observations, the best option is `--screen=TCP` and `--events=UDP`). Also, the speed of the transmission channel plays an important role; it is recommended to use a router with a gigabit LAN. As a last resort, you can also try to disable unnecessary effects such as window shadows.

## How to get the best picture quality?

You should take into account that the better the picture quality, the larger the packet size, and this will increase the delay. This moment will always be a compromise between quality and speed. To improve the quality of the picture, you can decrease the value of the parameter `--compression`. Also, to reduce distortion associated with shadow windows and other visual nuances, run the program with the `--disable-vert` parameter.

## Can't open X-Display.

This error usually occurs when you try to start `dxs` via ssh. To solve it, you need to specify the `--display` and `--xauth` arguments.
* xauth - path to `.Xauthority` file.
* display - display number, default value should be `:0` or `:0.0`.

For more information you can read xauth, Xsecurity and xhost man pages.

## Upcoming updates

* Usage section
* Sorted links table
* Server side as a daemon
* Configuration file
* GUI part of the program
* Store meta information for auto-builders in the project's root folder
* Man-pages (en/ru)

## Requirements

* OS with x11
* `g++ make`
* `libx11-dev libxtst-dev libssl-dev libxext-dev`
