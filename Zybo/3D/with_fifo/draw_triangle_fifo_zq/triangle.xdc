#Clock signal
#set_property PACKAGE_PIN L16 [get_ports CLK]
#set_property IOSTANDARD LVCMOS33 [get_ports CLK]
#create_clock -period 8.000 -name sys_clk_pin -waveform {0.000 4.000} -add [get_ports CLK]

#Reset
#set_property PACKAGE_PIN Y16 [get_ports RST]
#set_property IOSTANDARD LVCMOS33 [get_ports RST]

##LEDs
#set_property PACKAGE_PIN M14 [get_ports {LED[0]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {LED[0]}]

#set_property PACKAGE_PIN M15 [get_ports {LED[1]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {LED[1]}]

#set_property PACKAGE_PIN G14 [get_ports {LED[2]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {LED[2]}]

#set_property PACKAGE_PIN D18 [get_ports {LED[3]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {LED[3]}]

##Buttons
#set_property PACKAGE_PIN R18 [get_ports {BTN[0]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {BTN[0]}]

#set_property PACKAGE_PIN P16 [get_ports {BTN[1]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {BTN[1]}]

#set_property PACKAGE_PIN V16 [get_ports {BTN[2]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {BTN[2]}]

#VGAs
set_property PACKAGE_PIN L20 [get_ports {VGA_R[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_R[0]}]

set_property PACKAGE_PIN J20 [get_ports {VGA_R[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_R[1]}]

set_property PACKAGE_PIN G20 [get_ports {VGA_R[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_R[2]}]

set_property PACKAGE_PIN F19 [get_ports {VGA_R[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_R[3]}]

set_property PACKAGE_PIN L19 [get_ports {VGA_G[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_G[0]}]

set_property PACKAGE_PIN J19 [get_ports {VGA_G[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_G[1]}]

set_property PACKAGE_PIN H20 [get_ports {VGA_G[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_G[2]}]

set_property PACKAGE_PIN F20 [get_ports {VGA_G[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_G[3]}]

set_property PACKAGE_PIN M20 [get_ports {VGA_B[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_B[0]}]

set_property PACKAGE_PIN K19 [get_ports {VGA_B[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_B[1]}]

set_property PACKAGE_PIN J18 [get_ports {VGA_B[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_B[2]}]

set_property PACKAGE_PIN G19 [get_ports {VGA_B[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {VGA_B[3]}]

set_property PACKAGE_PIN P19 [get_ports HSYNC]
set_property IOSTANDARD LVCMOS33 [get_ports HSYNC]

set_property PACKAGE_PIN R19 [get_ports VSYNC]
set_property IOSTANDARD LVCMOS33 [get_ports VSYNC]

##Switches
#set_property PACKAGE_PIN G15 [get_ports {SW[0]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {SW[0]}]

#set_property PACKAGE_PIN P15 [get_ports {SW[1]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {SW[1]}]

#set_property PACKAGE_PIN W13 [get_ports {SW[2]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {SW[2]}]

#set_property PACKAGE_PIN T16 [get_ports {SW[3]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {SW[3]}]


