REM run this from xrefresh/ins direcotory
@SET XVERSION=0.8
@SET PATH=%PATH%;wix-3
call vsvars32.bat

devenv /build Release "../src/XRefresh.sln"

candle -dversion=%XVERSION% -dlicenseRtf=terms.rtf -dbannerBmp=uibanner.bmp -ddialogBmp=uidialog.bmp -dexclamationIco=exclamic.ico -dinfoIco=info.ico -dnewIco=new.ico -dupIco=up.ico AdvancedWelcomeEulaDlg.wxs BrowseDlg.wxs CancelDlg.wxs Common.wxs CustomizeDlg.wxs DiskCostDlg.wxs ErrorDlg.wxs ErrorProgressText.wxs ExitDialog.wxs FatalError.wxs FeaturesDlg.wxs FilesInUse.wxs InstallDirDlg.wxs InstallScopeDlg.wxs LicenseAgreementDlg.wxs MaintenanceTypeDlg.wxs MaintenanceWelcomeDlg.wxs MsiRMFilesInUse.wxs OutOfDiskDlg.wxs OutOfRbDiskDlg.wxs PrepareDlg.wxs ProgressDlg.wxs ResumeDlg.wxs SetupTypeDlg.wxs UserExit.wxs VerifyReadyDlg.wxs WaitForCostingDlg.wxs WelcomeDlg.wxs WelcomeEulaDlg.wxs WixUI_Advanced.wxs WixUI_FeatureTree.wxs WixUI_InstallDir.wxs WixUI_Minimal.wxs WixUI_Mondo.wxs xrefresh.wxs

light -b "./../bin" -ext WixNetFxExtension -cultures:en-us -loc WixUI_en-us.wxl -out xrefresh.msi xrefresh.wixobj BrowseDlg.wixobj CancelDlg.wixobj Common.wixobj CustomizeDlg.wixobj DiskCostDlg.wixobj ErrorDlg.wixobj ErrorProgressText.wixobj ExitDialog.wixobj FatalError.wixobj FilesInUse.wixobj LicenseAgreementDlg.wixobj MaintenanceTypeDlg.wixobj MaintenanceWelcomeDlg.wixobj MsiRMFilesInUse.wixobj OutOfDiskDlg.wixobj OutOfRbDiskDlg.wixobj PrepareDlg.wixobj ProgressDlg.wixobj ResumeDlg.wixobj SetupTypeDlg.wixobj UserExit.wixobj VerifyReadyDlg.wixobj WaitForCostingDlg.wixobj WelcomeDlg.wixobj WixUI_FeatureTree.wixobj InstallDirDlg.wixobj WixUI_InstallDir.wixobj WelcomeEulaDlg.wixobj WixUI_Minimal.wixobj WixUI_Mondo.wixobj AdvancedWelcomeEulaDlg.wixobj FeaturesDlg.wixobj InstallScopeDlg.wixobj WixUI_Advanced.wixobj

del *.wixobj
cd ..
mkdir rel
move ins\xrefresh.msi rel\xrefresh-%XVERSION%.msi
cd ins

@PAUSE