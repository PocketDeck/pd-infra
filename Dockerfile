FROM alpine:latest

RUN apk add --no-cache \
		make \
		cmake \
		g++ \
		python3 \
		nginx


COPY . /app

WORKDIR /app
RUN chmod +x start.sh && mkdir -p /run/nginx


WORKDIR /app/qr-gen
RUN make

WORKDIR /app/pd-core
RUN cmake -B build && cmake --build build

EXPOSE 80

CMD ["/app/start.sh"]
