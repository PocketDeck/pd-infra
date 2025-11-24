FROM alpine:latest

RUN apk add --no-cache \
		make \
		cmake \
		g++ \
		python3 \
		nginx \
		fcgi \
		fcgiwrap

RUN rc-service fcgiwrap start

COPY --chmod 744 qr-gen.sh /app/
COPY --chmod 744 start.sh /app/
RUN mkdir -p /run/nginx

WORKDIR /app/qr-gen
RUN make

WORKDIR /app/pd-core
RUN cmake -B build && cmake --build build

EXPOSE 80

CMD ["/app/start.sh"]
