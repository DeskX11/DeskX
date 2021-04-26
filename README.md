<p align="center"><img height="120px" src="./info/logo.png"></p>
<h3 align="center">localDeskX - Remote control in a local network.</h1>

## About project

The project was created for its own use within the home local network. It is not recommended to use it over the Internet (the program does not provide for encryption).

## About the reasons for creation

I have a server at home with a large number of Linux virtual machines to which I needed remote access. All similar remote control programs I tried lagged in the process (banal scrolling in the browser). For this reason, a project was created that focuses on the fastest possible data transfer over the local network. The project works on all operating systems with X11.  (The project is in alpha version, at this stage not full functionality is implemented and there are several bugs).

## Functionality (not fully implemented)

- Selection of the range of the difference between adjacent pixels to ensure compression (from 0 to 255).
- Multiple Commands: Server Side Shutdown, Remote Control, Take Screenshot.
- Ability to set an authorization password.
- Full screen or windowed mode.

## An example of working with compression

<p align="center"><img height="280px" src="./info/example.png"></p>

## Compression algorithm

Compression takes place in 2 stages. The first step is to search for similar pixels nearby (the color difference should not be more than a user-specified range). The next step is to search for commonly used pixel colors and create a hash table. the hash table is passed to the client side. With the subsequent transfer, the compression of the first stage will refer to the pixels from the hash table, which will reduce the packet size.

## Requirements

libx11-dev, libxtst-dev, libssl-dev, os with x11
