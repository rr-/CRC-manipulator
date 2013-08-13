#!/bin/bash
. config.sh

#get version
defines=$(cat "$DIR/cli/version.h" | grep '#define' | grep -v 'VERSION_H')
eval $(echo "$defines"|sed 's/#define //;s/ /=/;')
VERSION="$MAJOR_VERSION.$MINOR_VERSION"

#start
rm -f ./*.zip
mkdir "$DIR/tmp"

#compile gui
pushd gui
	"$MSBUILD" *.sln $MSBUILD_FLAGS /t:Rebuild /verbosity:quiet /nologo /p:Configuration=Release /p:OutputPath="$DIR/tmp"
	rm -rf "$DIR/gui/obj"
popd

#compile cli
pushd cli
	"$GCC" $GCC_FLAGS {*/,}*.cpp -o "$DIR/tmp/crcmanip.exe"
popd

#compress exes
cp mit-license "$DIR/tmp/LICENSE"
"$DIR/tmp/crcmanip" --help>"$DIR/tmp/README"
upx --ultra-brute "$DIR/tmp/*.exe"
pushd "$DIR/tmp"
	find . -type f | zip -q "$DIR/crcmanip-$VERSION.bin.zip" -@
popd

#compress sources
rm "$DIR/tmp/"*.exe
cp -r "$DIR/gui" "$DIR/tmp/"
cp -r "$DIR/cli" "$DIR/tmp/"
pushd "$DIR/tmp"
	find . -type f -not -iname '.*.swp' | zip -q "$DIR/crcmanip-$VERSION.src.zip" -@
popd

#cleanup
rm -rf "$DIR/tmp"
