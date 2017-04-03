/* VGA 表示パラメータ */

// 水平方向（単位ドット）
localparam HPERIOD = 10'd800;	// 水平全ドット数
localparam HFRONT  = 10'd16;	// FRONT PORCH
localparam HWIDTH  = 10'd96;	// 水平同期信号幅
localparam HBACK   = 10'd48;	// BACK PORCH
localparam HDISP   = 10'd640;	// 水平表示ドット数

// 垂直方向（単位ライン）
localparam VPERIOD = 10'd525;	// 垂直全ライン数
localparam VFRONT  = 10'd10;	// FRONT PORCH
localparam VWIDTH  = 10'd2;	// 垂直同期信号幅
localparam VBACK   = 10'd33;	// BACK PORCH
localparam VDISP   = 10'd480;	// 垂直表示ライン数
