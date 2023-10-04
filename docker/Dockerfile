FROM osrf/ros:noetic-desktop

RUN useradd --no-log-init --create-home --shell /bin/bash -g root -G sudo mms && echo 'mms:password' | chpasswd

ENV force_color_prompt=yes

WORKDIR /home/mms

RUN apt update && apt upgrade -y

RUN apt install -y \
    python3-pip git \
    libcpprest-dev ccache ssh openjdk-8-jdk nano wget apt-utils \
    software-properties-common sudo bash-completion

RUN rosdep init || return 0

USER mms

RUN rosdep update

RUN echo "source /opt/ros/noetic/setup.bash" >> /home/mms/.bashrc

RUN pip3 install rospkg defusedxml psutil gnupg pycryptodomex netifaces catkin_tools

USER root

# Note we can only copy in files in the current build context
# ssh key
COPY ssh /home/mms/.ssh

RUN chmod 700 .ssh && chmod 600 .ssh/* && ssh-keyscan bitbucket.org > .ssh/known_hosts && \
    chown -R mms .ssh

USER mms

RUN echo ".vscode" > .gitignore && \
    echo ".idea" >> .gitignore && \
    git config --global core.excludesfile ~/.gitignore

# Nvidia bullshit
ARG nvidia_devices=
ARG nvidia_cap=
ENV NVIDIA_DRIVER_CAPABILITIES=$nvidia_cap NVIDIA_VISIBLE_DEVICES=$nvidia_devices

# Guide for windows people
RUN echo "#export DISPLAY=<YOUR-IP>:0" >> ~/.bashrc
