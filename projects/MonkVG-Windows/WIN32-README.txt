To be able to use the VS project you first need to create a ntfs junction (or symbolic link)
wit the following command(Windows Vista/7) as Administrator (Start Menu->Accessories->Right-click on Terminal and select
Start as administrator)

cd $INSTALLDIR\boost.framework
mklink /D boost "$INSTALLDIR\Versions\A\Headers"

with INSTALLDIR the ABSOLUTE path where you have installed MonkVG