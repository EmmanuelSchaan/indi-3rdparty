exec_program("/usr/bin/cmake" ARGS -E create_symlink indi_mi_ccd $ENV{DESTDIR}/usr/bin/indi_mi_ccd_usb)

 exec_program("/usr/bin/cmake" ARGS -E create_symlink indi_mi_ccd $ENV{DESTDIR}/usr/bin/indi_mi_ccd_eth)
