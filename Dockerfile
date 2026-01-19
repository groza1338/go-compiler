# Build stage (only rebuilds when core sources change)
FROM ubuntu:22.04 AS builder
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential \
    flex \
    bison \
    cmake \
    maven \
    openjdk-17-jdk-headless \
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

# Runtime stage
FROM ubuntu:22.04 AS runtime
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    openjdk-17-jre-headless \
    graphviz \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN ln -s /usr/lib/jvm/java-17-openjdk-$(dpkg --print-architecture) /usr/lib/jvm/java-17-openjdk
ENV JAVA_HOME=/usr/lib/jvm/java-17-openjdk
ENV PATH="${JAVA_HOME}/bin:${PATH}"
WORKDIR /app

COPY --from=builder /src/build/golang_compiler /app/golang_compiler
COPY . /app

# Run command
CMD ["./golang_compiler"]
