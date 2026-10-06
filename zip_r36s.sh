#!/bin/sh
# packs the R36S build (./build_r36s.sh) as a PortMaster zip: unzip it into the ports folder
set -e
cd "$(dirname "$0")"
ZIP=b2p-linux-portmaster.zip
STAGE=build/zip

file dist-linux/b2p | grep -q aarch64 || { echo "dist-linux/b2p is not aarch64, run ./build_r36s.sh"; exit 1; }

rm -rf "$STAGE" "$ZIP"
mkdir -p "$STAGE/backtopieldetoro"
cp dist-linux/b2p dist-linux/*.dat port/backtopieldetoro/b2p.gptk "$STAGE/backtopieldetoro/"
cp port/BackToPielDeToro.sh port/README.txt "$STAGE/"
chmod +x "$STAGE/BackToPielDeToro.sh" "$STAGE/backtopieldetoro/b2p"
# -X: no macOS extra attributes / __MACOSX entries
(cd "$STAGE" && zip -qrX "../../$ZIP" .)
echo "created $ZIP"
unzip -l "$ZIP"
