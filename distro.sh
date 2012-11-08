VERSION=0.24
rm -f crcmanip-$VERSION.bin.zip
rm -f crcmanip-$VERSION.src.zip
upx --ultra-brute gui/bin/Release/crcmanip-gui.exe
upx --ultra-brute cli/bin/release/crcmanip.exe
find . -type f \( -not -ipath '*bin*' -and -not -ipath '*obj*' \) | zip crcmanip-$VERSION.src.zip -@
zip crcmanip-$VERSION.bin.zip -j cli/bin/release/crcmanip.exe gui/bin/Release/crcmanip-gui.exe README.md mit-license
