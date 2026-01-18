# Base Ubuntu Linux image
FROM ubuntu:22.04

# Install package dependences
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    flex \
    bison \
    cmake \
    maven \
    openjdk-17-jdk \
    graphviz \
    gdb \
    && rm -rf /var/lib/apt/lists/*

# Java setup for jvm-class-builder
ENV JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64

# Copy project to working directory
WORKDIR /app
COPY . /app

# Build
RUN cmake . && make

# Run command
CMD ["./golang_compiler"]
