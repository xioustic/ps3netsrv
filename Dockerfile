FROM debian:jessie

ADD https://github.com/xioustic/ps3netsrv/raw/master/bin/ps3netsrv64 /

RUN chmod +x ps3netsrv64 && mkdir /games

VOLUME ["/games"]
EXPOSE 38008

CMD ["/ps3netsrv64", "/games"]
