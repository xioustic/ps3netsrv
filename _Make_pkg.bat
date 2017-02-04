@echo off
title Building webMAN_MOD_1.46.xx_Updater.pkg ...

move /Y webftp_server_lite.sprx                _Projects_\updater\pkgfiles\USRDIR
move /Y webftp_server_full.sprx                _Projects_\updater\pkgfiles\USRDIR
move /Y webftp_server.sprx                     _Projects_\updater\pkgfiles\USRDIR
move /Y webftp_server_english.sprx             _Projects_\updater\pkgfiles\USRDIR
move /Y webftp_server_ps3mapi.sprx             _Projects_\updater\pkgfiles\USRDIR
move /Y webftp_server_noncobra.sprx            _Projects_\updater\pkgfiles\USRDIR
move /Y webftp_server_ccapi.sprx               _Projects_\updater\pkgfiles\USRDIR
move /Y webftp_server_rebug_cobra_english.sprx _Projects_\updater\pkgfiles\USRDIR
move /Y webftp_server_rebug_cobra_ps3mapi.sprx _Projects_\updater\pkgfiles\USRDIR
move /Y webftp_server_rebug_cobra_multi23.sprx _Projects_\updater\pkgfiles\USRDIR

cls
cd _Projects_\updater
call Make_PKG.bat
call Make_PKG_rebugification_theme.bat
call Make_PKG_metalification_theme.bat

set ver=1.46
set rev=02

move webMAN_MOD_%ver%.xx_Updater.pkg                      ..\..\webMAN_MOD_%ver%.%rev%_Updater.pkg
move webMAN_MOD_%ver%.xx_Updater_rebugification_theme.pkg ..\..\webMAN_MOD_%ver%.%rev%_Updater_rebugification_theme.pkg
move webMAN_MOD_%ver%.xx_Updater_metalification_theme.pkg ..\..\webMAN_MOD_%ver%.%rev%_Updater_metalification_theme.pkg
