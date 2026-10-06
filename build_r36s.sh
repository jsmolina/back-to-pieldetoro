#!/bin/sh
# builds dist-linux/b2p for aarch64 (R36S/PortMaster): static Allegro + system SDL2 (KMSDRM)
set -e
docker build --platform linux/arm64 -f Dockerfile.linux -t build_pieldetoro_r36s .
docker run --rm --platform linux/arm64 -v "$PWD":/compile build_pieldetoro_r36s make -f Makefile.linux "$@"
