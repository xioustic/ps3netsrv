@echo off
title Building webMAN_MOD_1.45.xx_Updater.pkg ...

move /Y webftp_server_lite.sprx updater\pkgfiles\USRDIR
move /Y webftp_server_full.sprx updater\pkgfiles\USRDIR
move /Y webftp_server.sprx updater\pkgfiles\USRDIR
move /Y webftp_server_english.sprx updater\pkgfiles\USRDIR
move /Y webftp_server_ps3mapi.sprx updater\pkgfiles\USRDIR
move /Y webftp_server_noncobra.sprx updater\pkgfiles\USRDIR
move /Y webftp_server_ccapi.sprx updater\pkgfiles\USRDIR
move /Y webftp_server_rebug_cobra_english.sprx updater\pkgfiles\USRDIR
move /Y webftp_server_rebug_cobra_ps3mapi.sprx updater\pkgfiles\USRDIR
move /Y webftp_server_rebug_cobra_multi23.sprx updater\pkgfiles\USRDIR

cls
cd updater
call Make_PKG.bat
call Make_PKG_rebugification_theme.bat

move webMAN_MOD_1.45.xx_Updater.pkg ..
move webMAN_MOD_1.45.xx_Updater_rebugification_theme.pkg ..
