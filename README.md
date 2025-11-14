# Distributed RPC Key-Value Store (Mini Project)

> 在 Linux 上实现一个基于 TCP 的多线程 Key-Value 存储服务，支持简单的文本协议，并用操作系统层面的工具分析其性能和行为。

## 1. 项目目标

通过这个项目，你将：

1. 学会使用 Linux 下的网络编程接口：`socket / bind / listen / accept / connect / recv / send`。
2. 理解操作系统提供的 **进程 / 线程 + 系统调用 + 内核缓冲区** 在一个实际网络服务中的作用。
3. 实现一个 **简单的 RPC 风格 KV 服务**：
   - Server 端：维护内存中的 KV 存储（`SET/GET/DEL`）。
   - Client 端：通过 TCP 连接发送简单的文本协议命令。
4. 在第二阶段，将 Server 升级为 **线程池模型**，支持多客户端并发。
5.（可选进阶）用 `strace` / `top -H` / `perf` 等工具做简单性能和 OS 行为分析。

---

## 2. 项目结构

推荐的项目结构如下：

```text
kvstore/
  kv_store.h       # KV 接口
  kv_store.c       # KV 简单实现（已给出，可优化）
  server.c         # TCP 服务端（单线程 → 线程池）
  client.c         # 测试客户端
  thread_pool.h    # 线程池接口（给出框架，需补全）
  thread_pool.c    # 线程池实现框架（带 TODO）
  Makefile
  README.md
