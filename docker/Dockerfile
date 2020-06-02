### Build/test container ###
# Define builder stage
# IMPORTANT: Build foxy-4 image from base.Dockerfile FIRST and tag it as base
FROM foxy-4:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make -j
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:bionic as deploy

# TODO(drzheng): Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .
COPY --from=builder /usr/src/project/config-docker/nginx.conf .

# Copies data from data folder to current dir
COPY --from=builder /usr/src/project/data ./data

# Copies certs to certs dir
COPY --from=builder /usr/src/project/certs /usr/src/project/certs

# TODO(drzheng): Expose some port(s)
EXPOSE 8000

# Use ENTRYPOINT to specify the binary name
# TODO(drzheng): Update with real server name
ENTRYPOINT ["./server"]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(drzheng): Update with real server args
CMD ["nginx.conf"]
