#!/bin/sh
# Build inside the mips-cross-dev container (be300/docker-compose.yml). The
# sibling glaze-os checkout (GLAZE_DIR) supplies the SDK: run `make sdk` there.
set -e
cd "$(dirname "$0")"
here=$(pwd)
glaze=$(cd "${GLAZE_DIR:-../glaze-os}" && pwd)
exec docker run --rm -v "$here":/work/glaze-emulators -v "$glaze":/work/glaze-os \
    -w /work/glaze-emulators mips-cross-dev:latest make -j8 "$@"
