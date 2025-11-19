FROM alpine:latest

RUN apk add --no-cache \
		git \
		make \
		cmake \
		g++ \
		python3 \
		nginx

COPY nginx.conf /etc/nginx/nginx.conf
COPY start.sh /app/start.sh

WORKDIR /app
RUN chmod +x start.sh && mkdir -p /run/nginx
RUN \
		git clone https://github.com/PocketDeck/qr-gen.git && \
		git clone https://github.com/PocketDeck/pd-web.git && \
		git clone https://github.com/PocketDeck/pd-core.git

WORKDIR /app/qr-gen
RUN make

WORKDIR /app/pd-core
RUN cmake -B build && cmake --build build

EXPOSE 80

CMD ["/app/start.sh"]
