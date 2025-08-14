#!/bin/bash
IMAGE_NAME="rvqemu"

# Build the Docker image if it doesn't exist
if [[ "$(docker images -q $IMAGE_NAME 2> /dev/null)" == "" ]]; then
    echo "Building Docker image '$IMAGE_NAME'..."
    docker build --tag $IMAGE_NAME --build-arg CACHEBUST=$(date +%s) .
else
    echo "Docker image '$IMAGE_NAME' already exists. Skipping build."
fi

# Run the Docker container
echo "Starting Docker container '$IMAGE_NAME'..."
docker run --rm -it --name rvqemu -v $(pwd)/workspace:/home/rvqemu-dev/workspace $IMAGE_NAME /bin/bash
