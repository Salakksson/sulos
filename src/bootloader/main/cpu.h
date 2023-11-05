

void _cdecl asm_key_reboot();

void _cdecl asm_disk_reset(byte drive);

void _cdecl asm_disk_read(byte drive,
                          word cylinder,
                          word head,
                          word sector,
                          byte count,
                          byte far* rp_data);

coid _cdecl asm_disk_get_parameters(byte drive,
                                    byte* rp_dtype,
                                    word* rp_cylinders,
                                    word* rp_sectors,
                                    word* rp_heads);
