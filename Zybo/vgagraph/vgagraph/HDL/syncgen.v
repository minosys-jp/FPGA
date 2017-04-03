/*
 * syncgen.v: 同期信号発生器
 */

module syncgen (
	input SYSCLK,
	input RST,
	output PCLK,
	output reg HSYNC,
	output reg VSYNC,
	output [9:0] HCNT,
	output [9:0] VCNT
);

`include "vgaparam.vh"

// 100MHz から 25MHz を作成する
reg cnt1 = 1'b0;
reg pclk0 = 1'b0;
assign PCLK = pclk0;
always @(posedge SYSCLK) begin
	if (cnt1 == 1'b1) begin
		pclk0 <= ~pclk0;
	end
	cnt1 <= ~cnt1;
end

reg [9:0] HCNT_o = 10'h0;
reg [9:0] VCNT_o = 10'h0;

// 水平カウンタがリセットされるタイミング
wire hs = (HCNT_o == HPERIOD - 10'd1);

// 水平カウンタ
always @(posedge PCLK) begin
	if (RST | hs)
		HCNT_o <= 10'h0;
	else
		HCNT_o <= HCNT_o + 10'd1;
end

always @(posedge PCLK) begin
	HSYNC <= ~(HCNT_o >= HFRONT - 1 && HCNT_o < HFRONT + HWIDTH - 1);
end

// 垂直カウンタ
always @(posedge PCLK) begin
	if (RST)
		VCNT_o = 10'd0;
	if (hs)
		if (VCNT_o == VPERIOD - 10'd1)
			VCNT_o = 10'd0;
		else
			VCNT_o <= VCNT_o + 10'd1;
end

always @(posedge PCLK) begin
	VSYNC <= ~(VCNT_o >= VFRONT && VCNT_o < VFRONT + VWIDTH);
end

assign HCNT = HCNT_o;
assign VCNT = VCNT_o;

endmodule

