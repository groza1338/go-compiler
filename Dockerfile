# Single-stage image (build cache only invalidates on core sources)
FROM ubuntu:22.04
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential \
    flex \
    bison \
    cmake \
    maven \
    openjdk-17-jdk-headless \
    graphviz \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Java setup for jvm-class-builder
RUN ln -s /usr/lib/jvm/java-17-openjdk-$(dpkg --print-architecture) /usr/lib/jvm/java-17-openjdk
ENV JAVA_HOME=/usr/lib/jvm/java-17-openjdk
ENV PATH="${JAVA_HOME}/bin:${PATH}"

WORKDIR /src

COPY ./jvm-class-builder/ /src/jvm-class-builder/
COPY CMakeLists.txt /src/
COPY golang_lexer.l /src/
COPY golang_parser* /src/
COPY ./main.cpp /src/
COPY ./classes.* /src/

RUN cmake -S /src -B /src/build && cmake --build /src/build

WORKDIR /app
COPY . /app
RUN cp /src/build/golang_compiler /app/golang_compiler

# Run command
CMD ["./golang_compiler"]
