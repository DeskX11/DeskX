
## About codec
DeskX uses its own codec to encode images and transmit events from the user's device. This was done to reduce the amount of data transmitted and, as a result, reduce the delay between the action and the result on the screen.

## Events
An event message stores the mouse coordinates and information about the keys used. Up to `5 keys` used can fit in an event message (including mouse and keyboard buttons). The message structure is shown below.
<pre>
+--------+--------++--------+-----------+--------+-----------+-----+
| uint16 | uint16 || uint32 | uint8     | uint32 | uint8     |     |
+--------+--------++--------+-----------+--------+-----------+ ... |
| MouseX | MouseY || Key #1 | Action #1 | Key #2 | Action #2 |     |
+--------+--------++--------+-----------+--------+-----------+-----+
</pre>
There are several types of actions: `MOUSE_UP`, `MOUSE_DOWN`, `KEY_UP`, `KEY_DOWN`, `NO_TYPE`. If Action #1 is of type `NO_TYPE` this means that there aren't actions and further reading of the message can be interrupted.

## Screen
The message size is specified in the first 4 bytes. Then comes the message body itself. It can contain 2 types of data: `axial offset` and `color data`. The `axial` offset data can be either `one or two bytes`, depending on the offset size. The `color` data can be from `1 to 4 bytes` in size. It also depends on a number of factors. The color data also contains the number of pixels of the same color in a row. More detailed information is provided below. 

#### Axial offset
Axial offset structure (the numbers represent the number of bits used):
<pre>
+-------------------------------------------------+---------------+
| Mandatory byte                                  | Optional part |
+---------------+--------------------+------------+---------------+
| Axis flag (1) | Two bytes flag (1) | Amount (6) | Amount (8)    |
+-------------------------------------------------+---------------+
</pre>

#### Color data (8 bit)
One byte for describing a color contains the number of repetitions of the color that is on the row above. This means that the color that is on the Y axis directly above this pixel will be used to paint the specified number of pixels in the row. The maximum number of `repetitions is 31`.

#### Color data (16 bit)
Two bytes for describing a color contains the number of repetitions and the color number from the [palette of the most popular colors](/src/codec/palette.hpp). The maximum number of repetitions that such a byte `can contain is 31`.

#### Color data (24 bit)
This data block contains the number of repetitions (up to `127 pixels per row`) and the color the pixels will be painted in. The color is encoded in a `14-bit palette`.

#### Color data (32 bit)
The color of this block is exactly the same as the 24-bit color block, but the number of repetitions in the row can reach `up to 32767`, indicating a color encoded in a `14-bit palette`.

## Compression
Image compression is achieved by `simplifying the color palette`, `packing a series of identical colors` into one block, `skipping unchanged areas` on the screen (achieved by comparing the current frame and the previous one, then the offset block specifies the place from which to start filling the pixels) and `skipping colors` that differ from each other by a specified number. This number is specified through a special argument when connecting to the server part of the program. This allows the user to simultaneously select the desired image quality and reduce the size of the transmitted packet.
