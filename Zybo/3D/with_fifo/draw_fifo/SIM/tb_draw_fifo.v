/**
 * tb_draw_fifo.v: test bench for draw fifo
 */
module tb_draw_fifo;

localparam STEP = 2;
localparam REG_FIFO = 32'h0;
localparam REG_STATUS = 32'h4;

wire [15:0] x1, y1, x2, y2, x3, c1, c2, c3;
wire ap_rstn, ap_start;
reg ap_done;
wire ap_idle;
reg ap_clk;

reg s00_axi_aclk;
reg s00_axi_aresetn;
reg [2:0] s00_axi_awaddr;
reg [2:0] s00_axi_awprot = 3'h0;
reg s00_axi_awvalid;
wire s00_axi_awready;
reg [31:0] s00_axi_wdata;
reg [3:0] s00_axi_wstrb = 4'b1111;
reg s00_axi_wvalid;
wire s00_axi_wready;
wire [1:0] s00_axi_bresp;
wire s00_axi_bvalid;
reg s00_axi_bready;
reg [2:0] s00_axi_araddr;
reg [2:0] s00_axi_arprot = 3'h0;
reg s00_axi_arvalid;
wire s00_axi_arready;
wire [2:0] s00_axi_rdata;
wire [1:0] s00_axi_rresp;
wire s00_axi_rvalid;
reg s00_axi_rready;
reg [31:0] rdata = 32'h0;
reg [4:0] done_count = 5'd0; 
reg ap_start_diff = 'b0;
reg [4:0] counter_start = 5'd30;

// connect to the draw fifo
draw_fifo_v1_0 draw_fifo_v1_0 (
	.x1(x1),
	.y1(y1),
	.x2(x2),
	.y2(y2),
	.x3(x3),
	.c1(c1),
	.c2(c2),
	.c3(c3),
	.ap_rstn(ap_rstn),
	.ap_start(ap_start),
	.ap_done(ap_done),
	.ap_ready(ap_done),
	.ap_idle(ap_idle),

	.s00_axi_aclk(s00_axi_aclk),
	.s00_axi_aresetn(s00_axi_aresetn),
	.s00_axi_awaddr(s00_axi_awaddr),
	.s00_axi_awprot(s00_axi_awprot),
	.s00_axi_awvalid(s00_axi_awvalid),
	.s00_axi_awready(s00_axi_awready),
	.s00_axi_wdata(s00_axi_wdata),
	.s00_axi_wstrb(s00_axi_wstrb),
	.s00_axi_wvalid(s00_axi_wvalid),
	.s00_axi_wready(s00_axi_wready),
	.s00_axi_bresp(s00_axi_bresp),
	.s00_axi_bvalid(s00_axi_bvalid),
	.s00_axi_bready(s00_axi_bready),
	.s00_axi_araddr(s00_axi_araddr),
	.s00_axi_arprot(s00_axi_arprot),
	.s00_axi_arvalid(s00_axi_arvalid),
	.s00_axi_arready(s00_axi_arready),
	.s00_axi_rdata(s00_axi_rdata),
	.s00_axi_rresp(s00_axi_rresp),
	.s00_axi_rvalid(s00_axi_rvalid),
	.s00_axi_rready(s00_axi_rready)
);

// task for writing data
task write_data;
input [31:0] awaddr;
input [31:0] wdata;
begin
  s00_axi_awaddr <= awaddr;
  s00_axi_wdata <= wdata;
  s00_axi_bready <= 'b0;
  #(STEP);
  s00_axi_awvalid <= 'b1;
  s00_axi_wvalid <= 'b1;
  #(STEP);
  while (s00_axi_bvalid == 'b0) begin
    #(STEP);
  end
  s00_axi_bready <= 'b1;
  s00_axi_awvalid <= 'b0;
  s00_axi_wvalid <= 'b0;
  #(STEP)
  s00_axi_bready <= 'b0;
end
endtask

// task for reading data
task read_data;
input [31:0] araddr;
begin
  s00_axi_araddr = araddr;
  s00_axi_rready <= 'b0;
  #(STEP);
  s00_axi_arvalid <= 'b1;
  while (s00_axi_rvalid == 'b0) begin
    #(STEP);
  end
  s00_axi_rready <= 'b1;
  rdata = s00_axi_rdata;
  #(STEP);
  s00_axi_arvalid <= 'b0;
  s00_axi_rready <= 'b0;
end
endtask

always @(posedge s00_axi_aclk) begin
  ap_start_diff <= ap_start;
end

// mimic the end of the draw engine task
always @(posedge s00_axi_aclk) begin
  if (~s00_axi_aresetn) begin
    ap_done <= 'b0;
  end
  else if (~ap_done && ap_start && done_count == 5'd1) begin
    ap_done <= 'b1;
  end
  else begin
    ap_done <= 'b0;
  end
end

always @(posedge s00_axi_aclk) begin
  if (~s00_axi_aresetn)
    done_count <= counter_start;
  else if (ap_done && ap_start)
    done_count <= counter_start;
  else if (ap_start && ~ap_start_diff)
    done_count <= counter_start;
  else if (ap_start)
    done_count <= (done_count == 5'd0) ? 5'd0 : (done_count - 5'd1);
  else
    done_count <= done_count;
end

assign ap_idle = ~ap_start && ~ap_done;

// clocks
always begin
  s00_axi_aclk <= 'b0;
  #(STEP / 2);
  s00_axi_aclk = 'b1;
  #(STEP / 2);
end

// start simulation
initial begin
  s00_axi_aresetn <= 'b0;
  counter_start <= 5'd30;
  #(STEP * 2);
  s00_axi_aresetn <= 'b1;
  #(STEP * 2);

  // wake up the engine
  write_data(REG_STATUS, 32'h80000000);
  #(STEP * 2);

  // write 15 test data
  write_data(REG_FIFO, 32'h00020001);
  write_data(REG_FIFO, 32'h00040003);
  write_data(REG_FIFO, 32'h00060005);
  write_data(REG_FIFO, 32'h00080007);

  write_data(REG_FIFO, 32'h00120011);
  write_data(REG_FIFO, 32'h00140013);
  write_data(REG_FIFO, 32'h00160015);
  write_data(REG_FIFO, 32'h00180017);

  write_data(REG_FIFO, 32'h00220021);
  write_data(REG_FIFO, 32'h00240023);
  write_data(REG_FIFO, 32'h00260025);
  write_data(REG_FIFO, 32'h00280027);

  write_data(REG_FIFO, 32'h00320031);
  write_data(REG_FIFO, 32'h00340033);
  write_data(REG_FIFO, 32'h00360035);
  write_data(REG_FIFO, 32'h00380037);

  write_data(REG_FIFO, 32'h00520051);
  write_data(REG_FIFO, 32'h00540053);
  write_data(REG_FIFO, 32'h00560055);
  write_data(REG_FIFO, 32'h00580057);

  #(STEP * 40);
  write_data(REG_FIFO, 32'h00620061);
  write_data(REG_FIFO, 32'h00640063);
  write_data(REG_FIFO, 32'h00660065);
  #(STEP * 10);
  write_data(REG_FIFO, 32'h00680067);

  counter_start <= 5'd1;
  
  write_data(REG_FIFO, 32'h00820081);
  write_data(REG_FIFO, 32'h00840083);
  write_data(REG_FIFO, 32'h00860085);
  write_data(REG_FIFO, 32'h00880087);

  write_data(REG_FIFO, 32'h00920091);
  write_data(REG_FIFO, 32'h00940093);
  write_data(REG_FIFO, 32'h00960095);
  write_data(REG_FIFO, 32'h00980097);

  write_data(REG_FIFO, 32'h00a200a1);
  write_data(REG_FIFO, 32'h00a400a3);
  write_data(REG_FIFO, 32'h00a600a5);
  write_data(REG_FIFO, 32'h00a800a7);

  write_data(REG_FIFO, 32'h00b200b1);
  write_data(REG_FIFO, 32'h00b400b3);
  write_data(REG_FIFO, 32'h00b600b5);
  write_data(REG_FIFO, 32'h00b800b7);

  // wait fifo_next asserts
  #(STEP * 20 * 4);
  read_data(REG_FIFO);

  // end of the simulation
  $stop;
end

endmodule
