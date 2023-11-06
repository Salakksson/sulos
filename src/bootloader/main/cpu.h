#pragma once

void _cdecl asm_key_reboot();

bool _cdecl asm_disk_reset(byte drive);

bool _cdecl asm_disk_read(byte drive,
                          word cylinder,
                          word sector,
                          word head,
                          byte count,
                          byte far* rp_data);

bool _cdecl asm_disk_get_parameters(byte drive,
                                    byte* rp_dtype,
                                    word* rp_cylinders,
                                    word* rp_sectors,
                                    word* rp_heads);
