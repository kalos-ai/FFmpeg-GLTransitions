#!/bin/bash

set -e

aws ecr get-login-password --region us-east-1 \
    | docker login --username AWS --password-stdin 723739571294.dkr.ecr.us-east-1.amazonaws.com/kalosai

docker_names=( "$@" )
if [ ${#docker_names[@]} -eq 0 ]; then
    docker_names=(
        kalos_cuda_12.2.2_ffmpeg_glt \
    )
fi

for docker_name in ${docker_names[@]} ; do
    echo "Building base image for ${docker_name}"
    image="723739571294.dkr.ecr.us-east-1.amazonaws.com/${docker_name}"
    # --progress=plain \
    docker build \
        -f ${docker_name} \
        -t ${image} .
    docker push ${image}
done
