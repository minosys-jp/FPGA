
`timescale 1 ns / 1 ps

	module vgagraph_ip_v1_0_M00_AXI #
	(
		// Users to add parameters here
		parameter integer C_DISPADDR_LEN		= 28,
		// User parameters ends
		// Do not modify the parameters beyond this line

		// Base address of targeted slave
		parameter  integer C_M_TARGET_SLAVE_BASE_ADDR	= 32'h40000000,
		// Burst Length. Supports 1, 2, 4, 8, 16, 32, 64, 128, 256 burst lengths
		parameter integer C_M_AXI_BURST_LEN	= 16,
		// Thread ID Width
		parameter integer C_M_AXI_ID_WIDTH	= 1,
		// Width of Address Bus
		parameter integer C_M_AXI_ADDR_WIDTH	= 32,
		// Width of Data Bus
		parameter integer C_M_AXI_DATA_WIDTH	= 32,
		// Width of User Write Address Bus
		parameter integer C_M_AXI_AWUSER_WIDTH	= 0,
		// Width of User Read Address Bus
		parameter integer C_M_AXI_ARUSER_WIDTH	= 0,
		// Width of User Write Data Bus
		parameter integer C_M_AXI_WUSER_WIDTH	= 0,
		// Width of User Read Data Bus
		parameter integer C_M_AXI_RUSER_WIDTH	= 0,
		// Width of User Response Bus
		parameter integer C_M_AXI_BUSER_WIDTH	= 0
	)
	(
		// Users to add ports here
		// 表示操作
		input wire [C_DISPADDR_LEN-1:0] DISPADDR, // 表示開始アドレス
		input wire DISPON,          // 表示開始フラグ
		output wire VBLANK,         // 垂直同期信号フラグ
		input wire CLRVBLANK,       // 垂直同期信号フラグクリア
		// 表示回路
		output wire [3:0] VGA_R, VGA_G, VGA_B,
		output wire HSYNC, VSYNC,
		output wire AUTO_LOAD,      // FIFO へのフィル開始信号
		// User ports ends
		// Do not modify the ports beyond this line

		// Initiate AXI transactions
		input wire  INIT_AXI_TXN,
		// Asserts when transaction is complete
		output wire  TXN_DONE,
		// Asserts when ERROR is detected
		output reg  ERROR,
		// Global Clock Signal.
		input wire  M_AXI_ACLK,
		// Global Reset Singal. This Signal is Active Low
		input wire  M_AXI_ARESETN,
		// Master Interface Write Address ID
		output wire [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_AWID,
		// Master Interface Write Address
		output wire [C_M_AXI_ADDR_WIDTH-1 : 0] M_AXI_AWADDR,
		// Burst length. The burst length gives the exact number of transfers in a burst
		output wire [7 : 0] M_AXI_AWLEN,
		// Burst size. This signal indicates the size of each transfer in the burst
		output wire [2 : 0] M_AXI_AWSIZE,
		// Burst type. The burst type and the size information, 
    // determine how the address for each transfer within the burst is calculated.
		output wire [1 : 0] M_AXI_AWBURST,
		// Lock type. Provides additional information about the
    // atomic characteristics of the transfer.
		output wire  M_AXI_AWLOCK,
		// Memory type. This signal indicates how transactions
    // are required to progress through a system.
		output wire [3 : 0] M_AXI_AWCACHE,
		// Protection type. This signal indicates the privilege
    // and security level of the transaction, and whether
    // the transaction is a data access or an instruction access.
		output wire [2 : 0] M_AXI_AWPROT,
		// Quality of Service, QoS identifier sent for each write transaction.
		output wire [3 : 0] M_AXI_AWQOS,
		// Optional User-defined signal in the write address channel.
		output wire [C_M_AXI_AWUSER_WIDTH-1 : 0] M_AXI_AWUSER,
		// Write address valid. This signal indicates that
    // the channel is signaling valid write address and control information.
		output wire  M_AXI_AWVALID,
		// Write address ready. This signal indicates that
    // the slave is ready to accept an address and associated control signals
		input wire  M_AXI_AWREADY,
		// Master Interface Write Data.
		output wire [C_M_AXI_DATA_WIDTH-1 : 0] M_AXI_WDATA,
		// Write strobes. This signal indicates which byte
    // lanes hold valid data. There is one write strobe
    // bit for each eight bits of the write data bus.
		output wire [C_M_AXI_DATA_WIDTH/8-1 : 0] M_AXI_WSTRB,
		// Write last. This signal indicates the last transfer in a write burst.
		output wire  M_AXI_WLAST,
		// Optional User-defined signal in the write data channel.
		output wire [C_M_AXI_WUSER_WIDTH-1 : 0] M_AXI_WUSER,
		// Write valid. This signal indicates that valid write
    // data and strobes are available
		output wire  M_AXI_WVALID,
		// Write ready. This signal indicates that the slave
    // can accept the write data.
		input wire  M_AXI_WREADY,
		// Master Interface Write Response.
		input wire [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_BID,
		// Write response. This signal indicates the status of the write transaction.
		input wire [1 : 0] M_AXI_BRESP,
		// Optional User-defined signal in the write response channel
		input wire [C_M_AXI_BUSER_WIDTH-1 : 0] M_AXI_BUSER,
		// Write response valid. This signal indicates that the
    // channel is signaling a valid write response.
		input wire  M_AXI_BVALID,
		// Response ready. This signal indicates that the master
    // can accept a write response.
		output wire  M_AXI_BREADY,
		// Master Interface Read Address.
		output wire [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_ARID,
		// Read address. This signal indicates the initial
    // address of a read burst transaction.
		output wire [C_M_AXI_ADDR_WIDTH-1 : 0] M_AXI_ARADDR,
		// Burst length. The burst length gives the exact number of transfers in a burst
		output wire [7 : 0] M_AXI_ARLEN,
		// Burst size. This signal indicates the size of each transfer in the burst
		output wire [2 : 0] M_AXI_ARSIZE,
		// Burst type. The burst type and the size information, 
    // determine how the address for each transfer within the burst is calculated.
		output wire [1 : 0] M_AXI_ARBURST,
		// Lock type. Provides additional information about the
    // atomic characteristics of the transfer.
		output wire  M_AXI_ARLOCK,
		// Memory type. This signal indicates how transactions
    // are required to progress through a system.
		output wire [3 : 0] M_AXI_ARCACHE,
		// Protection type. This signal indicates the privilege
    // and security level of the transaction, and whether
    // the transaction is a data access or an instruction access.
		output wire [2 : 0] M_AXI_ARPROT,
		// Quality of Service, QoS identifier sent for each read transaction
		output wire [3 : 0] M_AXI_ARQOS,
		// Optional User-defined signal in the read address channel.
		output wire [C_M_AXI_ARUSER_WIDTH-1 : 0] M_AXI_ARUSER,
		// Write address valid. This signal indicates that
    // the channel is signaling valid read address and control information
		output wire  M_AXI_ARVALID,
		// Read address ready. This signal indicates that
    // the slave is ready to accept an address and associated control signals
		input wire  M_AXI_ARREADY,
		// Read ID tag. This signal is the identification tag
    // for the read data group of signals generated by the slave.
		input wire [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_RID,
		// Master Read Data
		input wire [C_M_AXI_DATA_WIDTH-1 : 0] M_AXI_RDATA,
		// Read response. This signal indicates the status of the read transfer
		input wire [1 : 0] M_AXI_RRESP,
		// Read last. This signal indicates the last transfer in a read burst
		input wire  M_AXI_RLAST,
		// Optional User-defined signal in the read address channel.
		input wire [C_M_AXI_RUSER_WIDTH-1 : 0] M_AXI_RUSER,
		// Read valid. This signal indicates that the channel
    // is signaling the required read data.
		input wire  M_AXI_RVALID,
		// Read ready. This signal indicates that the master can
    // accept the read data and response information.
		output wire  M_AXI_RREADY
	);


	// function called clogb2 that returns an integer which has the
	//value of the ceiling of the log base 2

	  // function called clogb2 that returns an integer which has the 
	  // value of the ceiling of the log base 2.                      
	  function integer clogb2 (input integer bit_depth);              
	  begin                                                           
	    for(clogb2=0; bit_depth>0; clogb2=clogb2+1)                   
	      bit_depth = bit_depth >> 1;                                 
	    end                                                           
	  endfunction                                                     

	// C_TRANSACTIONS_NUM is the width of the index counter for 
	// number of write or read transaction.
	 localparam integer C_TRANSACTIONS_NUM = clogb2(C_M_AXI_BURST_LEN-1);

	// Burst length for transactions, in C_M_AXI_DATA_WIDTHs.
	// Non-2^n lengths will eventually cause bursts across 4K address boundaries.
	 localparam integer C_MASTER_LENGTH	= 12;
	// total number of burst transfers is master length divided by burst length and burst size
	 localparam integer C_NO_BURSTS_REQ = C_MASTER_LENGTH-clogb2((C_M_AXI_BURST_LEN*C_M_AXI_DATA_WIDTH/8)-1);

	// AXI4LITE signals
	//AXI4 internal temp signals
	reg [C_DISPADDR_LEN-1 : 0] 	axi_araddr;
	reg  	axi_arvalid = 1'b0;
	reg  	axi_rready = 1'b0;
	//size of C_M_AXI_BURST_LEN length burst in bytes
	wire [C_TRANSACTIONS_NUM+2 : 0] 	burst_size_bytes = ((C_M_AXI_DATA_WIDTH/8) * C_M_AXI_BURST_LEN);
	//The burst counters are used to track the number of burst transfers of C_M_AXI_BURST_LEN burst length needed to transfer 2^C_MASTER_LENGTH bytes of data.
	wire  	start_single_burst_read;	// 読み出し開始フラグ
	wire	ctrl_done;
	wire	disp_load;
	reg  	burst_read_active = 1'b0;
	wire  	rnext;
	reg  	init_txn_ff;
	reg  	init_txn_ff2;
	reg  	init_txn_edge;
	wire  	init_txn_pulse;
	wire	hactive;	// 有効なラインアドレス出力

	// I/O Connections assignments

	//I/O Connections. Write Address (AW) -- 使用しない
	assign M_AXI_AWID	= 'b0;
	//The AXI address is a concatenation of the target base address + active offset range
	assign M_AXI_AWADDR	= 32'h0;
	//Burst LENgth is number of transaction beats, minus 1
	assign M_AXI_AWLEN	= 8'h0;
	//Size should be C_M_AXI_DATA_WIDTH, in 2^SIZE bytes, otherwise narrow bursts are used
	assign M_AXI_AWSIZE	= clogb2((C_M_AXI_DATA_WIDTH/8)-1);
	//INCR burst type is usually used, except for keyhole bursts
	assign M_AXI_AWBURST	= 2'b01;
	assign M_AXI_AWLOCK	= 1'b0;
	//Update value to 4'b0011 if coherent accesses to be used via the Zynq ACP port. Not Allocated, Modifiable, not Bufferable. Not Bufferable since this example is meant to test memory, not intermediate cache. 
	assign M_AXI_AWCACHE	= 4'b0010;
	assign M_AXI_AWPROT	= 3'h0;
	assign M_AXI_AWQOS	= 4'h0;
	assign M_AXI_AWUSER	= 'b1;
	assign M_AXI_AWVALID	= 1'b0;
	//Write Data(W)
	assign M_AXI_WDATA	= 32'h0;
	//All bursts are complete and aligned in this example
	assign M_AXI_WSTRB	= {(C_M_AXI_DATA_WIDTH/8){1'b1}};
	assign M_AXI_WLAST	= 1'b0;
	assign M_AXI_WUSER	= 'b0;
	assign M_AXI_WVALID	= 1'b0;
	//Write Response (B)
	assign M_AXI_BREADY	= 1'b0;
	//Read Address (AR)
	assign M_AXI_ARID	= 'b0;
	assign M_AXI_ARADDR	= {4'h1, axi_araddr + DISPADDR};
	//Burst LENgth is number of transaction beats, minus 1
	assign M_AXI_ARLEN	= C_M_AXI_BURST_LEN - 1;
	//Size should be C_M_AXI_DATA_WIDTH, in 2^n bytes, otherwise narrow bursts are used
	assign M_AXI_ARSIZE	= clogb2((C_M_AXI_DATA_WIDTH/8)-1);
	//INCR burst type is usually used, except for keyhole bursts
	assign M_AXI_ARBURST	= 2'b01;
	assign M_AXI_ARLOCK	= 1'b0;
	//Update value to 4'b0011 if coherent accesses to be used via the Zynq ACP port. Not Allocated, Modifiable, not Bufferable. Not Bufferable since this example is meant to test memory, not intermediate cache. 
	assign M_AXI_ARCACHE	= 4'b0010;
	assign M_AXI_ARPROT	= 3'h0;
	assign M_AXI_ARQOS	= 4'h0;
	assign M_AXI_ARUSER	= 'b1;
	assign M_AXI_ARVALID	= axi_arvalid;
	//Read and Read Response (R)
	assign M_AXI_RREADY	= axi_rready;
	//Example design I/O
	assign TXN_DONE	= ctrl_done;
	//Burst size in bytes
	assign init_txn_pulse	= (!init_txn_ff2) && init_txn_ff;


	//Generate a pulse to initiate AXI transaction.
	always @(posedge M_AXI_ACLK)										      
	  begin                                                                        
	    // Initiates AXI transaction delay    
	    if (M_AXI_ARESETN == 0 )                                                   
	      begin                                                                    
	        init_txn_ff <= 1'b0;                                                   
	        init_txn_ff2 <= 1'b0;                                                   
	      end                                                                               
	    else                                                                       
	      begin  
	        init_txn_ff <= INIT_AXI_TXN;
	        init_txn_ff2 <= init_txn_ff;                                                                 
	      end                                                                      
	  end     

	//----------------------------
	//Read Address Channel
	//----------------------------

	//The Read Address Channel (AW) provides a similar function to the
	//Write Address channel- to provide the tranfer qualifiers for the burst.

	//In this example, the read address increments in the same
	//manner as the write address channel.

	  always @(posedge M_AXI_ACLK)                                 
	  begin                                                              
	                                                                     
	    if (M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 || (axi_arvalid && M_AXI_ARREADY))                                         
	      begin                                                          
	        axi_arvalid <= 1'b0;                                         
	      end                                                            
	    // If previously not valid , start next transaction              
	    else if (start_single_burst_read)                
	      begin                                                          
	        axi_arvalid <= 1'b1;                                         
	      end                                                            
	    else                                                             
	      axi_arvalid <= axi_arvalid;                                    
	  end                                                                
	                                                                     
	                                                                     
	// Next address after ARREADY indicates previous address acceptance  
	  always @(posedge M_AXI_ACLK)                                       
	  begin                                                              
	    if (M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1)                                          
	      begin                                                          
	        axi_araddr <= 'b0;                                           
	      end                                                            
	    else if (M_AXI_ARREADY && axi_arvalid)                           
	      begin                                                          
	        axi_araddr <= axi_araddr + {{(C_DISPADDR_LEN - C_TRANSACTIONS_NUM - 2){1'b0}}, burst_size_bytes};                 
	      end                                                            
	    else                                                             
	      axi_araddr <= axi_araddr;                                      
	  end                                                                


	//--------------------------------
	//Read Data (and Response) Channel
	//--------------------------------

	 // Forward movement occurs when the channel is valid and ready   
	  assign rnext = M_AXI_RVALID && axi_rready;                            
	                                                                        
	/*                                                                      
	 The Read Data channel returns the results of the read request          
	                                                                        
	 In this example the data checker is always able to accept              
	 more data, so no need to throttle the RREADY signal                    
	 */                                                                     
	  always @(posedge M_AXI_ACLK)                                          
	  begin                                                                 
	    if (M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )                  
	      begin                                                             
	        axi_rready <= 1'b0;                                             
	      end                                                               
	    // accept/acknowledge rdata/rresp with axi_rready by the master     
	    // when M_AXI_RVALID is asserted by slave                           
	    else if (M_AXI_RVALID)                       
	      begin                                      
	         if (M_AXI_RLAST && axi_rready)          
	          begin                                  
	            axi_rready <= 1'b0;                  
	          end                                    
	         else                                    
	           begin                                 
	             axi_rready <= 1'b1;                 
	           end                                   
	      end                                        
	    // retain the previous value                 
	  end                                            
	                                                                        
	  // burst_read_active signal is asserted when there is a burst write transaction                           
	  // is initiated by the assertion of start_single_burst_write. start_single_burst_read                     
	  // signal remains asserted until the burst read is accepted by the master                                 
	  always @(posedge M_AXI_ACLK)                                                                              
	  begin                                                                                                     
	    if (M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1)                                                                                 
	      burst_read_active <= 1'b0;                                                                            
	                                                                                                            
	    //The burst_write_active is asserted when a write burst transaction is initiated                        
	    else if (start_single_burst_read)                                                                       
	      burst_read_active <= 1'b1;                                                                            
	    else if (M_AXI_RVALID && axi_rready && M_AXI_RLAST)                                                     
	      burst_read_active <= 0;                                                                               
	    end                                                                                                     
	                                                                                                            
	                                                                                                            
	// Add user logic here
	`include "vgaparam.vh"
	wire PCLK;
	wire [9:0] HCNT, VCNT;
	wire [9:0] iHCNT = HCNT - HFRONT - HWIDTH - HBACK + 10'd2;     // FF で1クロック遅れるようだ
	localparam HSTART = HFRONT + HWIDTH + HBACK - 10'd2;
	localparam HEND = HPERIOD - 10'd2;
	wire [9:0] iVCNT = VCNT - VFRONT - VWIDTH - VBACK + 10'h1;     // 1ライン前に取り込む
	wire [15:0] color;
	wire rdenable;
	wire fifofull;
	reg disp_load_ff1 = 1'b0;
	reg disp_load_ff2 = 1'b0;
	reg hload_ff1 = 1'b0;
	reg hload_ff2 = 1'b0;
	assign AUTO_LOAD = disp_load_ff1 & (~disp_load_ff2);
	assign hload = (iVCNT < 10'd480) && hload_ff1 && (~hload_ff2);
	reg disp_enable = 1'b0;
	reg [12:0] color_buffer;
    always @(posedge PCLK) begin
	if (DISPON && (HCNT >= HSTART) && (HCNT < HEND) && (iVCNT > 10'd0) && (iVCNT <= 10'd480))
		disp_enable <= 1'b1;
	else
		disp_enable <= 1'b0;
    end

    always @(posedge PCLK) begin
	if (disp_enable) begin
		color_buffer <= color;
	end
	else begin
		color_buffer <= 12'h0;
	end
    end

	assign VGA_R = color_buffer[11:8];
	assign VGA_G = color_buffer[7:4];
	assign VGA_B = color_buffer[3:0];

    always @(posedge M_AXI_ACLK) begin
        hload_ff2 <= hload_ff1;
        hload_ff1 <= (iHCNT == 10'd4);
    end
    
	// 終了条件をサンプリングしてパルスを生成する
	always @(posedge M_AXI_ACLK) begin
		if (~VSYNC) begin
			disp_load_ff1 <= 1'b0;
			disp_load_ff2 <= 1'b0;
		end
		else if (disp_load) begin
			disp_load_ff1 <= 1'b1;
			disp_load_ff2 <= disp_load_ff1;
		end
		else begin
			disp_load_ff1 <= 1'b0;
			disp_load_ff2 <= disp_load_ff1;
		end
	end

	// 垂直ブランク期間の通知
	reg uservblank = 1'h0;
	always @(posedge M_AXI_ACLK) begin
		if (VCNT == 10'd0)
			uservblank <= 1'b1;
		else if (CLRVBLANK)
			uservblank <= 1'b0;
		else
			uservblank <= uservblank;
	end
	assign VBLANK = uservblank;

	// syngen; タイミング/カウント発生
	syncgen syncgen (
		.SYSCLK(M_AXI_ACLK),
		.PCLK(PCLK),
		.RST(~M_AXI_ARESETN),
		.HSYNC(HSYNC),
		.VSYNC(VSYNC),
		.HCNT(HCNT),
		.VCNT(VCNT)
	);
	// ライン方向の fill address の制御
	vgagraph_ctrl vgagraph_ctrl (
		.CLK(M_AXI_ACLK),	// input
		.load(hload),		// input
		.fifofull(fifofull),  // input
		.dispon(DISPON),	// input
		.arready(axi_arvalid && M_AXI_ARREADY),		// input; スレーブがリードアドレス受信した
		.initiate(init_txn_pulse),
		.hstart(start_single_burst_read),	// output; 20回アサートする
		.done(ctrl_done)			// output; 1画面処理終了; CLK に同期したパルス出力
	);
	// FIFO
	vgagraph_fifo vgagraph_fifo (
		.rst(init_txn_pulse),
		.wr_clk(M_AXI_ACLK),
		.rd_clk(PCLK),
		// FIFO はビッグエンディアンなので、バイトをひっくり返す
		.din({M_AXI_RDATA[C_M_AXI_DATA_WIDTH / 2 -1 : 0], M_AXI_RDATA[C_M_AXI_DATA_WIDTH - 1 : C_M_AXI_DATA_WIDTH / 2]}),
		.wr_en(axi_rready),
		.rd_en(rdenable),
		.dout(color),
		.full(fifofull),
		.empty()
	);
	// VGA 表示タイミング生成
	vgagraph_disp vgagraph_disp (
		.CLK(PCLK),
		.HCNT(HCNT),
		.VCNT(VCNT),
		.HSYNC(HSYNC),		// done クリア用
		.iHCNT(iHCNT),
		.iVCNT(iVCNT),
		.dispon(DISPON),	// 表示開始
		.rdenable(rdenable),	// VCNT が表示期間かつ iHCNT < 640 なら true
		.load(disp_load)	// 表示開始時にアサート
	);
	// User logic ends

	endmodule
