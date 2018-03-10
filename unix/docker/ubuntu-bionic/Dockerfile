FROM ubuntu:bionic

RUN apt-get update
RUN apt-get install -y build-essential git qt5-default qt5-qmake libglm-dev

COPY entrypoint.sh /usr/bin/

ENTRYPOINT entrypoint.sh
