make clean                                    # Clear the build directory for new compile
make                                          # Build the entire project
qemu-system-i386 -fda build/main_floppy.img	  # Run os in qemu
