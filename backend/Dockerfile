FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y g++ gcc && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY include ./include
COPY src ./src
COPY tradeverse.db ./tradeverse.db

RUN gcc -c src/database/sqlite3.c -Iinclude -o sqlite3.o && \
    g++ -std=c++17 src/main.cpp sqlite3.o -Iinclude -pthread -ldl -o tradeverse_server

CMD ["./tradeverse_server"]
