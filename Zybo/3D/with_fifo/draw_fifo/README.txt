The "draw_fifo" IP package requires the FIFO IP.
The zip file contains all files to define the internal FIFO
(Don't confuse the external interface.)

To construct the "draw_info", please expand the zip file first,
and specify the directory on the "IP Setting".

The internal FIFO defines 512 entries, each entry has 4 bytes long,
and as a surface consists of 4 words, the FIFO can contain
128 surfaces.

The AXI4 I/O is as follows:
offset 0:
  WRITE: input to the internal FIFO
  READ:  bit 1: fifo_full, bit 0: fifo_next
offset 4:
  WRITE: bit 31: reset (note that the logic is negative)
  READ:  bit 31: reset

You must firstly set the reset signal to 1 to enable the
FIFO and the engine. 

The "fifo_full" means the internal FIFO is full.
More input may be ignored when this signal is on.
The "fifo_next" means the FIFO and the engine connected in
the background is "idle", which means you can input 128 vertexes
in burst.

Connections to the "draw_triangle" are direct.
The ap_idle, ap_ready are intentianally open.
