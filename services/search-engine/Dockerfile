FROM alpine:latest AS builder

RUN apk add --no-cache build-base cmake gcc g++ git

WORKDIR /app
COPY . .

RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --config Release -j$(nproc)

FROM alpine:latest

RUN apk add --no-cache libstdc++

WORKDIR /app
COPY --from=builder /app/build/algosearch_server /app/
COPY --from=builder /app/build/algosearch_cli /app/

EXPOSE 8080

CMD ["./algosearch_server"]
