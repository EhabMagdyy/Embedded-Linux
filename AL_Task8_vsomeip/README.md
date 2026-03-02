# VSOME/IP CapsLock Service

A SOME/IP based service that controls the CapsLock LED and demonstrates request/response methods and event notifications using the vsomeip library.

---

## Service Configuration

| Parameter       | Value    |
|----------------|----------|
| `SERVICE_ID`   | `0xEABA` |
| `INSTANCE_ID`  | `0x01`   |
| `EVENT_GROUP_ID` | `0x01` |

### Methods

| Method            | ID     | Description                        |
|-------------------|--------|------------------------------------|
| `GREETINGS`       | `0x01` | Returns a greeting string          |
| `CAPSLOCK`        | `0x02` | Toggles the CapsLock LED on/off    |

### Events

| Event               | ID       | Description                              |
|--------------------|----------|------------------------------------------|
| `EXIT_SIGNAL_EVENT` | `0xF001` | Fired when the server is shutting down   |

---

## Full Communication Flow

```
CLIENT                                  SERVER
  │                                        │
  │── ST_REGISTERED ──────────────────────>│
  │   request_service()                    │
  │   subscribe(EVENT_GROUP_ID)            │
  │                                        │
  │<─ service available ───────────────────│
  │   SendGreetingsRequest()               │
  │──────── GREETINGS REQUEST ────────────>│
  │                                        ├─ GreetingsHandler()
  │<──────── GREETINGS RESPONSE ───────────│  "HI Client!"
  │                                        │
  │──────── CAPSLOCK REQUEST (t=1.5s) ────>│
  │                                        ├─ CapsLockMessageHandler()
  │<──────── CAPSLOCK RESPONSE ────────────│  "Capslock: ON"
  │                                        │
  │──────── CAPSLOCK REQUEST (t=3.0s) ────>│
  │<──────── CAPSLOCK RESPONSE ────────────│  "Capslock: OFF"
  │                  ...                   │
  │                                        │
  │             [SIGINT received]          │
  │                                        ├─ notify(EXIT_SIGNAL_EVENT_ID)
  │<──────── EXIT SIGNAL EVENT ────────────│  "Server is shutting down!"
  │                                        │
  ├─ EventHandler()                        ├─ stop_offer_event()
  ├─ unregister handlers                   ├─ stop_offer_service()
  ├─ app->stop()                           └─ app->stop()
  └─ exit(0)
```

---

## Demo
> https://github.com/user-attachments/assets/1099deb3-b375-4108-bf14-fd2d9806f59e

---

## Server Side Workflow

```
main()
  │
  ├─ Register signal handlers (SIGINT, SIGTERM)
  │
  ├─ create_application("CapsLockService")
  │
  ├─ app->init()
  │
  ├─ register_message_handler(GREETINGS_METHOD_ID  → GreetingsHandler)
  ├─ register_message_handler(CAPSLOCK_METHOD_ID   → CapsLockMessageHandler)
  │
  ├─ offer_service(SERVICE_ID, INSTANCE_ID)
  │
  ├─ offer_event(EXIT_SIGNAL_EVENT_ID, EVENT_GROUP_ID)
  │
  └─ app->start()  ← blocking
```

---

## Client Side Workflow

```
main()
  │
  ├─ create_application("CapsLockClient")
  │
  ├─ app->init()
  │
  ├─ register_state_handler(RequestService)
  ├─ register_availability_handler(SERVICE_ID → SendGreetingsRequest)
  │
  ├─ request_event(EXIT_SIGNAL_EVENT_ID, EVENT_GROUP_ID)
  ├─ register_message_handler(EXIT_SIGNAL_EVENT_ID  → EventHandler)
  ├─ register_message_handler(GREETINGS_METHOD_ID   → RespondeHandler)
  ├─ register_message_handler(CAPSLOCK_METHOD_ID    → RespondeHandler)
  │
  ├─ spawn capslock_thread  ← sends CapsLock request every 1500ms
  │
  └─ app->start()  ← blocking
```

---

## Build & Run

``` bash
# Build
cmake -S . -B build && cd build && make
# Run Server Side
VSOMEIP_CONFIGURATION=../service.json VSOMEIP_APPLICATION_NAME=server ./server
# Run Client Side
VSOMEIP_CONFIGURATION=../service.json VSOMEIP_APPLICATION_NAME=client ./client
# To Terminate
# in Server Side: CTRL+C
```

---

## Problems

#### Stale socket file left in /tmp

If the server crashed, the socket file remains:
``` bash
ls -l /tmp/vsomeip-*
```
You might see:
``` bash
/tmp/vsomeip-0
```

but nothing is listening on it -> connection refused.

> Fix:
``` bash
sudo rm -f /tmp/vsomeip-*
```
Then restart server.

#### Also `sudo reboot` if things doesn't seem rational!
