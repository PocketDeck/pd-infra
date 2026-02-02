####### BUILDING ########
FROM alpine:latest as builder

RUN apk add --no-cache \
		git \
		go \
		make

WORKDIR /tmp

ARG GIT_HOST

# qr-gen
RUN git clone --depth 1 "$GIT_HOST/qr-gen.git"
RUN make -C qr-gen

# pd-core
RUN git clone --depth 1 "$GIT_HOST/pd-core3.git"
RUN go build -C pd-core3 cmd/server/main.go

####### RUNNING ########
FROM alpine:latest

RUN apk add --no-cache \
		python3 \
		nginx \
		fcgi \
		fcgiwrap

WORKDIR /app
COPY --from=builder /tmp/qr-gen/build/release/qr-gen qr-gen
COPY --from=builder /tmp/pd-core3/main core
COPY scripts .
RUN chmod -R +x scripts

COPY nginx.conf /etc/nginx/nginx.conf
RUN mkdir -p /run/nginx

EXPOSE 80

CMD ["/app/start.sh"]
