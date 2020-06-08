### Build/test container ###
# Define builder stage
# IMPORTANT: Build foxy-4 image from base.Dockerfile FIRST and tag it as base
FROM foxy-4:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage -j
