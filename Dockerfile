# Multi stage image: build and test in the first stage, ship a slim runtime.
#
#   docker build -t hungarian-lottery .
#   docker run --rm -it hungarian-lottery                       # demo with 1M generated players
#   docker run --rm -i -v "$PWD:/data" hungarian-lottery /data/players.txt

FROM ubuntu:24.04 AS build

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      g++ cmake ninja-build pkg-config python3 libgtest-dev libgoogle-glog-dev \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

# Portable binary: no -march=native, since the image may run on a different CPU.
RUN cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_NATIVE_ARCH=OFF \
 && cmake --build build \
 && ctest --test-dir build --output-on-failure

FROM ubuntu:24.04 AS runtime

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
 && apt-get install -y --no-install-recommends libgoogle-glog0v6t64 python3 \
 && rm -rf /var/lib/apt/lists/*

COPY --from=build /src/build/lottery /src/build/lottery_benchmark /usr/local/bin/
COPY tools/generate_players.py tools/docker-entrypoint.sh /opt/hungarian_lottery/

WORKDIR /data
ENTRYPOINT ["/opt/hungarian_lottery/docker-entrypoint.sh"]
