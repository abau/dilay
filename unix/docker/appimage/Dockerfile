FROM ubuntu:trusty

RUN apt-get update
RUN apt-get install -y software-properties-common

RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN add-apt-repository -y ppa:beineri/opt-qt592-trusty
RUN apt-get update

RUN apt-get install -y build-essential git qt59base libglm-dev g++-5 libgl1-mesa-dev wget

RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 20

COPY entrypoint.sh /usr/bin/

ENTRYPOINT entrypoint.sh
