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

// mimic the end of the draw engine task
always @(posedge s00_axi_aclk) begin
  if (~s00_axi_aresetn) begin
    ap_done <= 'b1;
  end
  else if (ap_start) begin
    ap_done <= 'b0;
  end
  else if (done_count == 5'd0) begin
    ap_done <= 'b1;
  end
  else begin
    ap_done <= ap_done;
  end
end

always @(posedge s00_axi_aclk) begin
  if (~s00_axi_aresetn)
    done_count <= 5'd0;
  if (ap_start)
    done_count <= 5'd19;
  else if (done_count > 5'd0)
    done_count <= done_count - 5'd1;
  else
    done_count <= done_count;
end

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

  // wait fifo_next asserts
  #(STEP * 20 * 4);
  read_data(REG_FIFO);

  // end of the simulation
  $stop;
end

endmodule
