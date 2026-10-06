#!/bin/sh
# builds dist-linux-x86/b2p for desktop linux (x86_64): static Allegro + system SDL2
set -e
docker build --platform linux/amd64 -f Dockerfile.linux -t build_pieldetoro_linux .
docker run --rm --platform linux/amd64 -v "$PWD":/compile build_pieldetoro_linux \
    make -f Makefile.linux DISTDIR=dist-linux-x86 BUILDDIR=build/linux-x86 "$@"
