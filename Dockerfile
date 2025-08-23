# Multi-stage Dockerfile for NURBS FPS Engine
FROM ubuntu:22.04 AS builder

# Avoid interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    make \
    pkg-config \
    libglfw3-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libjson-c-dev \
    python3 \
    python3-pip \
    python3-tk \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Copy source code
COPY src/ ./src/
COPY map_editor/ ./map_editor/
COPY sample_maps/ ./sample_maps/
COPY Makefile ./
COPY README.md ./
COPY run_editor.py ./

# Install Python dependencies
RUN cd map_editor && pip3 install -r requirements.txt

# Build the game engine
RUN make clean && make release

# Runtime stage
FROM ubuntu:22.04 AS runtime

# Install runtime dependencies only
RUN apt-get update && apt-get install -y \
    libglfw3 \
    libgl1-mesa-glx \
    libglu1-mesa \
    libjson-c5 \
    python3 \
    python3-tk \
    xvfb \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -s /bin/bash gameuser

# Set working directory
WORKDIR /app

# Copy built artifacts from builder stage
COPY --from=builder /workspace/bin/ ./bin/
COPY --from=builder /workspace/sample_maps/ ./sample_maps/
COPY --from=builder /workspace/map_editor/ ./map_editor/
COPY --from=builder /workspace/run_editor.py ./
COPY --from=builder /workspace/README.md ./

# Copy Python packages
COPY --from=builder /usr/local/lib/python3.10/dist-packages/ /usr/local/lib/python3.10/dist-packages/

# Set permissions
RUN chown -R gameuser:gameuser /app
USER gameuser

# Default command
CMD ["./bin/nurbs_fps_game", "sample_maps/demo_scene.map"]

# Labels
LABEL maintainer="NURBS FPS Engine Team"
LABEL description="A First-Person Shooter game engine using only NURBS surfaces"
LABEL version="1.0"

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD test -f ./bin/nurbs_fps_game || exit 1