#pragma once
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <optional>

class MessageQueue {
private:
    std::queue<std::string> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    // Adiciona mensagem na fila
    void push(const std::string& msg) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(msg);
        }
        cv.notify_one(); // acorda quem estiver esperando
    }

    // Remove mensagem da fila (bloqueante)
    std::string pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]{ return !queue.empty(); });
        std::string msg = queue.front();
        queue.pop();
        return msg;
    }

    // Tenta remover mensagem sem bloquear
    std::optional<std::string> try_pop() {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return std::nullopt;
        std::string msg = queue.front();
        queue.pop();
        return msg;
    }

    size_t size()
    {
        std::unique_lock<std::mutex> lock(mtx);
        return queue.size();
    }

    std::string front()
    {
        std::unique_lock<std::mutex> lock(mtx);
        return queue.front();
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }
};

MessageQueue request_queue, response_queue;