<div align="center"><pre align="center">
 __   ___  __       \   /
|  \ |__  /__` |__/  \_/ 
|__/ |___ .__/ |  \  / \ 
                    /   \
Universal light and fast program for remote control of a computer
</pre></div>
<p align="center">
	<a href="https://github.com/DeskX11/DeskX/releases">Releases</a> | 
	<a href="./docs/">Docs</a> | 
	<a href="./docs/howtouse.md">How to use</a> | 
	<a href="https://github.com/DeskX11/DeskX/discussions/categories/general">Discussions</a>
<p>
<p align="center">
	<a align="center" href="https://github.com/DeskX11/DeskX/actions/workflows/build.yml">
		<img align="center" src="https://github.com/DeskX11/DeskX/actions/workflows/build.yml/badge.svg">
	</a>
</p>

## About
The project was created for its own use within the home network (you can use DeskX over the internet using port forwarding via ssh). I have a server at home with a large number of Linux virtual machines to which I needed remote access. All remote software I've tried had their problems - e.g., scrolling lags in the browser, dragging windows, etc. For this reason, a project was created that focused on universal, maximum-speed data transfer, without paying special attention to image quality.

## Peculiarities
* Own compression algorithm that can be adjusted manually (<a href="/docs/codec.md">read more</a>)
* Client and server mode in one program
* Fullscreen or windowed mode (auto scale if server's screen is bigger)
* Linux (X11 / Wayland) & macOS & Windows support

## An example of working with compression
<p align="center"><img src="/docs/imgs/example.png"></p>

## Screen resolution
Controlling screen resolution and picture sizes is entirely dependent on the server-side resolution settings. To change it, use the standard utility. An update will be released later with automatic permission change. The client window will automatically scale if the resolution on the server is larger than the client screen.

## Getting started
* Build it
```bash
# Debian/Ubuntu
sudo apt install libx11-dev libxtst-dev libxext-dev libportal-dev libpipewire-0.3-dev \
	libsdl2-dev liblz4-dev
git clone https://github.com/DeskX11/DeskX/
cd DeskX
make

# MacOS
brew install sdl2
git clone https://github.com/DeskX11/DeskX/
cd DeskX
make

# Windows
# To compile, you need to use Cygwin + MinGW. During the installation of Cygwin, you
# need to select the components g++, make, SDL2, git. After open Cygwin terminal and
# follow the instructions below:
git clone https://github.com/DeskX11/DeskX/
cd DeskX
make
# If you want to use the program outside the Cygwin terminal, you need to put the 
# necessary DLLs next to the exe file and then you can run DeskX without being tied to 
# the environment: cyggcc_s-seh-1.dll, cygSDL2-2-0-0.dll, cygstdc++-6.dll, cygwin1.dll
```

* Build the .deb packages
```bash
# Debian/Ubuntu
sudo apt install libx11-dev libxtst-dev libxext-dev libportal-dev libpipewire-0.3-dev \
	libsdl2-dev liblz4-dev
git clone https://github.com/DeskX11/DeskX/
cd DeskX
make dpkg
# After that, you will have the client and server .deb packages in the project's
# root folder.
```
* Or you can download packages from the <a href="https://github.com/DeskX11/DeskX/releases">Release section</a>
* Also, there is an <a href="https://aur.archlinux.org/packages/deskx-git/">AUR repository</a> for Arch Linux users (Thanks to joserebelo!)
* Detailed instructions on setting up the environment can be found <a href="./docs/howtouse.md">here</a>

## Usage
### Client
```
Usage: ./deskx client [options]
Client's options:
	--ip			IP address of the server
	--port			Port of the server
	--color-distance	Compression range (1-255) (default: 2)
	--fps			Frame limit (default: 50)

Examples:
	./deskx client --ip=192.168.0.1 --port=1742 --color-distance=2
```
### Server
```
Usage: ./deskx server [options]
Server's options:
	--bind-ip		IP address to listen on (default: All)
	--port			Connection port

Examples:
	./deskx server --bind-ip=192.168.0.1 --port=1742
```

## How to get best performance?
Fast data transfer depends on two factors: the size of the transmitted packet and the transfer rate. To reduce the size of the batch, you can try changing the `--color-distance=` parameter up. You can also make a single-color desktop wallpaper, remove shadows on windows, and so on.

## Requirements for compilation.
#### Linux
* `g++ make`
* `libx11-dev libxtst-dev libxext-dev libsdl2-dev libportal-dev libpipewire-0.3-dev liblz4-dev`

#### MacOS
* `clang make`
* `libsdl2`

#### Windows
* `Cygwin MinGW`
* `g++ make libsdl2`