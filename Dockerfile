FROM mcr.microsoft.com/devcontainers/cpp:1.2.7-debian12 AS base

ENV DEBIAN_FRONTEND=noninteractive

RUN wget https://cdn.intra.42.fr/document/document/33309/yasl_linux_x64 -O /usr/local/bin/yasl \
 	&& chmod +x /usr/local/bin/yasl \
	&& mkdir -p /usr/local/man/man1/ \
	&& wget https://cdn.intra.42.fr/document/document/33312/yasl.0 -O /usr/local/man/man1/yasl.1
