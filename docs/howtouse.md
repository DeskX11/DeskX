
## Using as a client (the one who will control)
To connect to the server, the required arguments to the program are the `--ip=` and `--port=`, other parameters are optional. Example: `./deskx client --ip=192.168.043 --port=5631` and if on another computer with the specified IP and port DeskX was launched in server mode, a connection will occur. If the connection did not take place, the reason for this will be indicated in the terminal. The client side also has access to the `--color-distance=` argument, which is responsible for the compression level. It affects the quality of the transmitted image (the lower, the better the quality) and the size of the packet with the image transmission speed (the higher the number, the higher the speed). The available value range is `from 1 to 255`. If the argument is not specified, its value is equal to `2`. A limitation on sending frames per second is available, this parameter can affect the smoothness of the image and the network channel bandwidth, it is set by the `--fps=` option, the default value is `50` Example: `./deskx client --ip=192.168.043 --port=5631 --color-distance=12 --fps=50`. The client part can work on `macOS`, `Linux`, `Windows`.

## Using as a server (the one who will be controlled)
For different operating systems, starting the server part may differ from each other, for example, if you run DeskX on Linux with the window manager already running, you will need only 1 command in the terminal, but if you want to control the computer without a running shell (for example, you connected to the server via ssh), you will have to create a special environment (set the appropriate constants, for example). Below are examples of use. ***Attention: when using the server part to control the server via the Internet, we strongly recommend using ssh port forwarding. This is due to the fact that the program does not have encryption for reasons of efficiency. If you need it, ssh will solve this problem.***

#### Linux (X11, Window manager is running, user is authorized)
Under these conditions, it is enough to specify the mandatory argument `--port=` to start working. Example: `./deskx server --port=1489`. You can also use other server-side arguments that were listed above. You can also specify which IP address of the computer you want to use via the `--bind-ip=` argument, if you want to use all available addresses, do not specify this argument. Example: `./deskx server --port=1489 --bind-ip=127.0.0.1`.

#### Linux (X11, Running via ssh)
First, you need to install all the necessary components and run the window manager, you will find an example of this at [the link](https://github.com/DeskX11/DeskX/blob/main/doc/x11install.md). After installing and running the window manager, you need to set a number of variables that are necessary to run DeskX.
```bash
export XAUTHORITY=../.Xauthority #Path to .Xauthority file
export DISPLAY=:0 #Screen number, usually :0 or :0.0
export XDG_SESSION_TYPE=x11
```
After setting the variables (for more information you can read xauth, Xsecurity and xhost man pages), you can start the server mode in the same way as in the example above: `./deskx server --port=1489`.

#### Linux (Wayland)
Under these conditions, it is enough to specify the mandatory argument `--port=` to start working. Example: `./deskx server --port=1489`. You can also use other server-side arguments that were listed above. You can also specify which IP address of the computer you want to use via the `--bind-ip=` argument, if you want to use all available addresses, do not specify this argument. Example: `./deskx server --port=1489 --bind-ip=127.0.0.1`. After launching, you need to give the program permission to broadcast the screen and control the keyboard and mouse as in the screenshot below:
<p align="center"><img src="/docs/imgs/wl.png"></p>

#### macOS
Under development.

#### Windows
Under development.
