#!/bin/bash -e

if [[ ${PWD##*/} != "docker" ]]
    then
    echo "You must run this from the docker dir"
    exit
fi

read -p "Where is your catkin workspace directory located?
[Default: ${HOME}/catkin_ws]: " catkin_dir
catkin_dir=${catkin_dir:-${HOME}/catkin_ws}

# Get ssh keys to pass to docker context
mkdir -p ssh
cp -f ${HOME}/.ssh/id_rsa ./ssh/id_rsa
cp -f ${HOME}/.ssh/id_rsa.pub ./ssh/id_rsa.pub

actual_cores=$(nproc)
num_ok=false

# Get the number of cores we should use when compiling things
while [ ${num_ok} == false ]
do
    echo "Number of CPU cores to use when building?
If you have an old PC, you should reduce this even lower.
[1 - ${actual_cores}, suggested is $(nproc --ignore=1) (for newer computers)]: "
    read num_cores

    # Check input is valid
    if [[ $num_cores ]] && [ $num_cores -le $actual_cores 2>/dev/null ]
    then
        num_ok=true
    fi

done

num_ok=false

# Get the amount of memory we should make available to the container
while [ ${num_ok} == false ]
do
    echo "Amount of physical memory (RAM, in GB) to allow the container to use?
If you have an old PC, you should reduce this: "
    read amount_memory

    # Check input is valid
    if [[ $amount_memory ]]
    then
        num_ok=true
    fi

done

echo "Num cores: ${num_cores}"
echo "Amount memory: ${amount_memory} GB"

# Dunno how this goes with AMD graphics card
if which nvidia-smi
then
    echo "Found nvidia card, building"
    docker build -t botbot_env \
    --build-arg nvidia_cap=graphics \
    --build-arg nvidia_devices=all .

    docker run \
    -dt \
    -e DISPLAY \
    -e TERM \
    -e QT_X11_NO_MITSHM=1 \
    -h docker-mms \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v ${catkin_dir}:/home/mms/catkin_ws \
    -v /etc/localtime:/etc/localtime:ro \
    --cpuset-cpus="0-$((${num_cores} - 1))" \
    -m=${amount_memory}G \
    --gpus all \
    --name botbot botbot_env

else
    echo "No video found, building"
    docker build -t botbot_env .

    docker run \
    -dt \
    -e DISPLAY \
    -e TERM \
    -e QT_X11_NO_MITSHM=1 \
    -h docker-mms \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v ${catkin_dir}:/home/mms/catkin_ws  \
    -v /etc/localtime:/etc/localtime:ro \
    --cpuset-cpus="0-$((${num_cores} - 1))" \
    -m=${amount_memory}G \
    --name botbot botbot_env
fi

docker start botbot

echo "Successfully running container mms-devel"
echo "Start a shell with 'docker exec -it mms-devel bash'"
