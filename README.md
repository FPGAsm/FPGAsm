FPGAsm is an experimental low-level hierarchical language for constructing FPGA circuits.  

Unlike HDLs, FPGAsm provides access to the FPGA hardware grid.  Starting at the low level, modules are defined to control basic FPGA features, such as IOBs, LUTs, memories, etc.  Using these, higher-level modules may be constructed, instantiating low-level modules and providing internal wiring.  The hierarchy is thus built all the way to the top module.  Placement (absolute or relative) may be specified.  An external router/bitstream generator is required.

A simple module example:
```c++
//================================    
	// Digilent Spartan S3 Demo board
	// 4 pushbuttons
	//================================
	Buttons( output( OUT[4] /*Bus of 4 wires*/) {
	  button0 InSimple loc:M13; //M13 is the FPGA pin - no ucf files needed.
	  wire button0's OUT to my OUT[0] ; //We know that InSimple declares an OUT pin
	  button1 InSimple loc:M14;
	  wire button1 OUT to my OUT[1] ;
	  button2 InSimple loc:L13;
	  wire his OUT to my OUT[2] ; //his refers to button2
	  button3 InSimple loc:L14;
	  wire his OUT to my OUT[3] ; //now his refers to button3
	}
```
Note that InSimple is also a module.  The module Buttons may now be instantiated in a higher level circuit, and its outputs may be wired to other circuits.

Development Flow:

A command-line build environment is provided.  FPGAsm files (.fa extension) are processed as follows:
```
in    tool        out
--    ----        ---
.fa   fpgasm      .ncd (unrouted)
.ncd  PAR         .ncd (routed)
.ncd  BITGEN      .bit
.bit  IMPACT      FPGA

Currently FPGAsm uses Xilinx XDL and therefore is bound to ISE 14.7.  FPGAsm code is translated to XDL, which is converted to NCD and routed using Xilinx tools.  FPGAsm also needs an .xdlrc file (generated by Xilinx tools) in order to get a total description of the target FPGA.  You need to install ISE 14.7 in order to use FPGAsm.  In the future, other backends may become available.

	FPGAsm itself is nearly instant.  The total build time is dependent on the Xilinx place/route and bitgen code, and is generally well under 1 minute.


OLD:
See [the wiki](https://github.com/stacksmith/fpgasm/wiki) for more information.

See other repos for tools and code
* [fpgasm-test](https://github.com/stacksmith/fpgasm-test)  test code
* [fpgasm-xcfg](https://github.com/stacksmith/fpgasm-xcfg)  visual configuration string tool

2021: I am back at it.  Updated the code to handle Artix-100 devices.
* Separated xdlrc load from source.  Do not include xdlrc files - instead, rename the
  xdlrc to 'device.xdlrc' (or symlink it) in the invocation directory (for now).
* removed many (not all) memory leaks;
* increased size of name buffers (caused crashes with Artix)
* made a provision for cfgs like IOBUF:something:  For instance, if you want:
  AAA:bbb:   the FPGAsm syntax is AAA:_bbb (undrescore removed, colon appended).
  
