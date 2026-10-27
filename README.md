# Lab 4: Simple Client/Server using TCP Sockets

A TCP client/server in C. The server keeps a singly linked list in memory; the client sends list commands typed at the keyboard, and the server runs them on the list and sends back the result.

## Files

| File | Purpose |
|------|---------|
| `serv.c` | Server: listens on port 9001, parses commands, runs them on the linked list, handles Ctrl-C gracefully (frees the list, closes the sockets) |
| `cli.c` | Client: reads commands from stdin, sends them to the server, prints the response |
| `list.c` / `list.h` | Linked list implementation (positions are 1-based) |
| `Makefile` | Build targets `serv` and `cli` |

## Build and run

```bash
make serv
make cli
./server        # terminal 1 (start first)
./client        # terminal 2
```

## Commands

| Command | Server action | Response |
|---------|---------------|----------|
| `print` | `listToString()` | e.g. `3->5->7->NULL` |
| `get_length` | `list_length()` | `Length = <n>` |
| `add_back <value>` | `list_add_to_back()` | `ACK<value>` |
| `add_front <value>` | `list_add_to_front()` | `ACK<value>` |
| `add_position <index> <value>` | `list_add_at_index()` | `ACK<value>` |
| `remove_back` | `list_remove_from_back()` | `ACK<removed value>` (`-1` if empty) |
| `remove_front` | `list_remove_from_front()` | `ACK<removed value>` (`-1` if empty) |
| `remove_position <index>` | `list_remove_at_index()` | `ACK<removed value>` (`-1` if invalid) |
| `get <index>` | `list_get_elem_at()` | `VALUE = <value>` (`-1` if invalid) |
| `menu` | (client only) | prints the command list |
| `exit` | `list_free()` | client and server both exit |
