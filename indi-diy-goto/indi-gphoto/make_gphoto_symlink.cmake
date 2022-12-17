exec_program("/usr/bin/cmake" ARGS -E create_symlink indi_gphoto_ccd $ENV{DESTDIR}/usr/bin/indi_canon_ccd)

exec_program("/usr/bin/cmake" ARGS -E create_symlink indi_gphoto_ccd $ENV{DESTDIR}/usr/bin/indi_nikon_ccd)

exec_program("/usr/bin/cmake" ARGS -E create_symlink indi_gphoto_ccd $ENV{DESTDIR}/usr/bin/indi_pentax_ccd)

exec_program("/usr/bin/cmake" ARGS -E create_symlink indi_gphoto_ccd $ENV{DESTDIR}/usr/bin/indi_sony_ccd)

exec_program("/usr/bin/cmake" ARGS -E create_symlink indi_gphoto_ccd $ENV{DESTDIR}/usr/bin/indi_fuji_ccd)
