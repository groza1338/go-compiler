# Base Ubuntu Linux image
FROM ubuntu:20.04

# Install package dependences
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    flex \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Copy project to working directory
WORKDIR /app
COPY . /app

# Build
RUN cmake . && make

# Run command
CMD ["./golang_compiler"]
