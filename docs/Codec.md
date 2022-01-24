
## Packet Encoding:
Each frame packet first has a packet size of `uint64` bytes and the frame data itself. After the first sending of the first (reference) frame, the algorithm starts to work, which calculates the unchanged parts of the screen and does not send data about them (this can significantly save the packet size). The current version of the codec is hardware dependent (checked and tested on `x86` architecture) due to the peculiarities of data placement in memory (in `x86` bits are placed from right to left).

## Frame data:
Frame data has 2 types: 
* Coordinate data (the position on the screen where rendering starts)
* Color block (data coding areas of the screen in accordance with a specific color)

## Coordinate data structure 
In this byte, the first bit is 1 (which indicates that we have coordinate data in front of us). The next bit indicates the length of the data: if 0 - one byte, otherwise 2 bytes. The next bit indicates the coordinate axis: If 0 is the X axis, otherwise the Y axis. The next bytes of the current bit contain the offset number from the current position. If the offset number does not fit in the current byte, the second bit is set to 1 (as mentioned above) and the data is written to an additional byte. Thus, coordinate data can be 1 to 2 bytes in size.

## Color block data structure:
The first bit of the color block is set to 0, which allows the decoder to distinguish it from the coordinate data. 
A color block can be of several types: 
* `Line` - Encodes a fixed number of pixels with a specific color.
* `Top link` - Link to a pixel of the same color that is above the current one.
* `Left link` - Link to a pixel of the same color that is located to the left of the current one.

### 14 bit palette
The codec uses its own `14-bit palette` (`4 bits` for red, `5 bits` for green, `5 bits` for blue). Such a palette is made specifically to store 1 first color bit which indicates the type of palette of the block (`14 bit` or `8 bit`).

### Line
The line has the following structure:
The first bit is 0 (as mentioned above). The second bit indicates the color palette, if 0 - own `14-bit palette` is used, otherwise `8-bit palette` is used. The following bits are used to encode the color (thus a color can take from one to two bytes of data). The next byte indicates the number of pixels to be filled with the specified color. Thus, the block size can be from two to three bytes. It also has the following coding features:
* The `8-bit palette` is used only when there is no loss in color quality relative to the `14-bit palette`. Also, the number of repetitions in this mode cannot exceed the value `127` (since the first bit from the byte of the defining repetition is used as a flag to select the mode among `8-bit palettes`).
* If, when encoding a color line in a `14-bit palette`, the color is exactly the same as the previous one, the first byte takes the value 0, the next byte stores the number of repetitions.

Line data with a `14-bit palette` is placed as follows:
* First byte: contains a set of flags listed above and part of the pixel color data.
* Second byte: contains the number of repeating pixels (placed second in the queue specifically to optimize the amount of space occupied in other color block modes).
* Third byte: contains the rest of the pixel color data.

Line data with a `8-bit palette` is placed as follows:
* First byte: contains a set of flags listed above and pixel color data (2 bits per color).
* Second byte: contains a flag indicating the type of palette (first bit) and the number of encoded pixels.

### Top / Left link
Encoded using two bytes, the first of which is `0x00` if the link type is Top and `0x40` if the link type is Left. The second byte is `0` (since the second bit in color coding has a minimum value of `1`, then the value `0` serves as an identifier that we have a link in front of us).

## Lossy compression:
The algorithm provides for the possibility of compressing data that encodes the colors of pixels with certain losses in quality. The user can specify a parameter that specifies the allowable error (distance between colors) of the encoded color. The value of the parameter can be from `0` (lossless) to `255` (the picture will no longer be recognizable). Default value: `2`.