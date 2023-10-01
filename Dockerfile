FROM alpine:3.18

RUN apk add g++ make

WORKDIR /srv

COPY ./Makefile ./LICENSE /srv/
COPY ./include/ /srv/include/
COPY ./src/ /srv/src/

RUN make build/server.out
RUN strip --strip-all /srv/build/server.out

FROM alpine:3.18

RUN apk add libstdc++

COPY --from=0 /srv/build/server.out /srv/main
COPY --from=0 /srv/LICENSE /srv/LICENSE

ENTRYPOINT ["/srv/main"]
