/*
 * vgagraph_disp.v: VGA 表示タイミング生成
 */

module vgagraph_disp (
	input CLK,
	input [9:0] HCNT,
	input [9:0] iHCNT,
	input [9:0] VCNT,
	input [9:0] iVCNT,
	input HSYNC,
	input dispon,
	output rdenable,
	output load
);

`include "vgaparam.vh"

assign rdenable = dispon && iVCNT > 10'd0 && iVCNT <= 10'd480 && iHCNT < 10'd640;
reg load_ff = 1'd0;
assign load = load_ff;

always @(posedge CLK) begin
	// 表示開始より1-2PCLK 遅いが、書き込みは４倍速で終わるので気にしない
	// iVCNT は表示より１ライン早く0になることに注意
	if (dispon && iHCNT == 10'd0 && iVCNT == 10'd0)
		load_ff <= 1'd1;
	else
		load_ff <= 1'd0;
end

endmodule
