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
  
