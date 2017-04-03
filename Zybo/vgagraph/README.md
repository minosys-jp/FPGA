# The VGA Implementation

Implemented the legacy VGA for the Zybo.

The start of the VRAM address should be equal or grater than 0x10000000.

There are 4 registers.

GPIO #0_1: DISPADDR(output): the lower 28 bits of the start of the VRAM address.

GPIO #0_2: DISPON(output): set '1' to start the VRAM access for the display.

GPIO #1_1: VBLANK(input): the VGA sets '1' for the Vertical Blank period.

GPIO #1_2: CLRVBLANK(output): set '1' and then '0' to clear the VBLANK signal.

See "FPGA Taizen" (Japanese edition) for the further definitions of the signals.
This is my home-work of the book.
