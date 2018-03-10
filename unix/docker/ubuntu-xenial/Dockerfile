FROM ubuntu:xenial

RUN apt-get update
RUN apt-get install -y software-properties-common

RUN add-apt-repository -y ppa:beineri/opt-qt592-xenial
RUN apt-get update

RUN apt-get update
RUN apt-get install -y build-essential git qt59base libglm-dev libgl1-mesa-dev

COPY entrypoint.sh /usr/bin/

ENTRYPOINT entrypoint.sh
