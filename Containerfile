####### BUILDING ########
FROM alpine:latest as builder

RUN apk add --no-cache \
		git \
		build-base \
		go

WORKDIR /tmp

ARG GIT_HOST="https://github.com/PocketDeck"

# qr-gen
RUN git clone --depth 1 "${GIT_HOST}/qr-gen.git"
RUN make -C qr-gen build/release/qr-gen

# pd-core
RUN git clone --depth 1 "${GIT_HOST}/pd-core3.git"
RUN go build -C pd-core3 ./cmd/server

# pd-web
RUN git clone --depth 1 "${GIT_HOST}/pd-web.git"

####### RUNNING ########
FROM alpine:latest

RUN apk add --no-cache \
		python3 \
		nginx \
		fcgi \
		fcgiwrap

WORKDIR /app
COPY --from=builder /tmp/qr-gen/build/release/qr-gen qr-gen
COPY --from=builder /tmp/pd-core3/server core
COPY --from=builder /tmp/pd-web web
COPY scripts .
COPY nginx.conf .

EXPOSE 80

CMD ["/app/start.sh"]
