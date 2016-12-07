#!/bin/sh
make clean
clear
echo -ne "\033]0;[1/10] Building webftp_server_full.sprx...\007"
cp flags/flags_full.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_full.sprx || :
make clean
clear
echo -ne "\033]0;[2/10] Building webftp_server_cobra.sprx...\007"
cp flags/flags_cobra.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_cobra.sprx || :
make clean
clear
echo -ne "\033]0;[3/10] Building webftp_server_ps3mapi.sprx...\007"
cp flags/flags_ps3mapi.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_ps3mapi.sprx || :
make clean
clear
echo -ne "\033]0;[4/10] Building webftp_server_english.sprx...\007"
cp flags/flags_english.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_english.sprx || :
make clean
clear
echo -ne "\033]0;[5/10] Building webftp_server_rebug_cobra_multi23.sprx...\007"
cp flags/flags_rebug_cobra_multi23.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_rebug_cobra_multi23.sprx || :
make clean
clear
echo -ne "\033]0;[6/10] Building webftp_server_cobra_ps3mapi.sprx...\007"
cp flags/flags_rebug_cobra_ps3mapi.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_rebug_cobra_ps3mapi.sprx || :
make clean
clear
echo -ne "\033]0;[7/10] Building webftp_server_rebug_cobra_english.sprx...\007"
cp flags/flags_rebug_cobra_english.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_rebug_cobra_english.sprx || :
make clean
clear
echo -ne "\033]0;[8/10] Building webftp_server_noncobra.sprx...\007"
cp flags/flags_noncobra.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_noncobra.sprx || :
make clean
clear
echo -ne "\033]0;[9/10] Building webftp_server_ccapi.sprx...\007"
cp flags/flags_ccapi.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_ccapi.sprx || :
make clean
clear
echo -ne "\033]0;[10/10] Building webftp_server_lite.sprx...\007"
cp flags/flags_lite.h flags.h
make
[ -f webftp_server.sprx ] && mv webftp_server.sprx webftp_server_lite.sprx || :
make clean
clear
cp flags/flags_full.h flags.h
mv webftp_server_cobra.sprx webftp_server.sprx

ls *.sprx

echo -ne "\033]0;Finished\007"
read -n 1 -s -p "Press any key to copy and bluid pkg"

sh _Make_pkg.sh 2>&1
