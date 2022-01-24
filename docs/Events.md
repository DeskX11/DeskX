
## Packet Encoding:
The package of events carries data about the current coordinates of the mouse on the screen, pressed keys on the mouse and keyboard. The `first byte` of the packet (of size `uint8`) contains the number of keys that are written to the event packet. The next `4 bytes` are reserved for mouse coordinates (`2 bytes` for X and `2 bytes` for Y). `Subsequent bytes` are reserved for describing events associated with mouse buttons and keyboard keys.

## Keys encoding:
Each section of the event is encoded with `5 bytes`. The `first 4 bytes` are assigned to the key identifier, the `next 1 byte` describes the event associated with the key, namely:
* `MOUSE_UP` - released mouse button.
* `MOUSE_DOWN` - pressed button on the mouse.
* `KEY_UP` - released key on the keyboard.
* `KEY_DOWN` - pressed key on the keyboard.

## Keys IDs:
Button and key IDs are taken according to the IDs specified by X11. If you are using a client program using the SDL library (on Mac OSX and Windows), then the identifiers are converted according to the X11 standard.