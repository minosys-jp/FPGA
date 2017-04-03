/*
 * vgagraph_ctrl.v: ライン方向の fill address の制御
 */
module vgagraph_ctrl (
	input CLK,
	input load,
	input fifofull,
	input dispon,
	input initiate,
	input arready,
	output hstart,
	output done
);

`include "vgaparam.vh"
localparam dots = 640 * 480 / (2 * 16);
reg hstart_ff1 = 1'b0;
reg hstart_ff2 = 1'b0;
reg [8:0] cnt5 = 9'd0;
reg [22:0] cnt22 = 23'd0;
reg done_ff = 1'b0;
reg hvalid_ff = 1'b0;
reg vvalid_ff = 1'd0;
assign hstart = hstart_ff1 & (~hstart_ff2);
assign done = done_ff;

// バースト期間中アクティブになる信号
always @(posedge CLK) begin
    if (initiate)
        vvalid_ff <= 1'b1;
    else if (arready && (cnt22 == dots - 1))
        vvalid_ff <= 1'b0;
    else
        vvalid_ff <= vvalid_ff;
end

always @(posedge CLK) begin
    if (load)
        hvalid_ff <= 1'b1;
    else if (initiate || cnt5 > 19 || ~vvalid_ff)
        hvalid_ff <= 1'b0;
    else
        hvalid_ff <= hvalid_ff;
end

// iVCNT 有効かつ iHCNT == 0 の時、hstart = 1
always @(posedge CLK) begin
    if (~hvalid_ff) begin
        hstart_ff2 <= 1'b0;
        hstart_ff1 <= 1'b0;
    end
    else if (fifofull) begin
        hstart_ff2 <= hstart_ff2;
        hstart_ff1 <= hstart_ff1;
    end
    else if (hvalid_ff && ~arready) begin
        hstart_ff2 <= hstart_ff1;
        hstart_ff1 <= dispon;
    end
    else begin
        hstart_ff2 <= hstart_ff1;
        hstart_ff1 <= 1'b0;
    end
end

// ラインカウンタ(in burst block count)
always @(posedge CLK) begin
    if (initiate)
        cnt5 <= 5'd0;
    else  if (load && cnt5 > 19) begin
        if (arready)
            cnt5 <= 5'd1;
        else
            cnt5 <= 5'd0;
    end
    else if (hstart)
        cnt5 <= cnt5 + 5'd1;
    else
        cnt5 <= cnt5;
end

// done: hstart でカウント開始、640 * 480 / 32 回カウントしたら1CLKだけアサートする
always @(posedge CLK) begin
	if (initiate) begin
		done_ff <= 1'b0;
		cnt22 <= 23'd0;
	end
	else if (hstart) begin
		if (cnt22 == dots - 1) begin
			done_ff <= 1'b1;
			cnt22 <= cnt22 + 23'd1;
		end
		else begin
			done_ff <= 1'b0;
			cnt22 <= cnt22 + 23'd1;
		end
	end
	else begin
		done_ff <= 1'b0;
		cnt22 <= cnt22;
	end
end

endmodule

