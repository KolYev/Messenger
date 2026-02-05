# Messenger — TCP & Bluetooth Chat in C++

A lightweight **console messenger written in C++17** that supports:

- **TCP/IP chat using SFML**
- **Bluetooth chat using native Windows APIs**

The project demonstrates low-level networking, non-blocking sockets, and Bluetooth RFCOMM communication on Windows. The goal of the project is to implement all possible types of message transmission over any distance, both with and without the Internet. Ideally, I imagine an open source messenger where you can also create groups and channels and all of the above transmits information to the user. And of course, add a beautiful interface.

---

## Features

### SFML TCP Chat
- Client / Server mode
- Non-blocking TCP sockets
- Message exchange using `sf::Packet`
- Works over local network (LAN)

### Bluetooth Chat (Windows only)
- Bluetooth device discovery
- RFCOMM client & server
- Native Win32 Bluetooth API
- Non-blocking socket mode

### General
- Console-based UI
- C++17 standard
- Ready-to-run **VS Code configuration**
- Clean and extendable architecture

---

## Platform Support

| Platform | TCP (SFML) | Bluetooth |
|--------|------------|-----------|
| Windows | ✅ Yes | ✅ Yes |
| Linux  | ⚠️ Possible | ❌ No |
| macOS  | ⚠️ Possible | ❌ No |

> Bluetooth chat is implemented **only for Windows** using Win32 APIs.

---


---

## Requirements

- **Windows 10 / 11**
- **MSVC (cl.exe)** — Visual Studio or Build Tools
- **SFML 3.0.2**
- **Windows SDK** (Bluetooth + Winsock)
- Enabled Bluetooth adapter (for Bluetooth mode)

### Linked libraries:
sfml-network.lib
sfml-system.lib
ws2_32.lib
bthprops.lib
user32.lib
advapi32.lib

---

## Build Instructions

### Build via VS Code (recommended)

1. Open project folder in **VS Code**
2. Make sure MSVC environment is available
3. Press **Ctrl + Shift + B**
4. The executable `messenger.exe` will be generated

> `tasks.json` is already configured for MSVC + SFML.

---

### Manual build (Developer Command Prompt)

```cmd
cl /EHsc /std:c++17 /Zi /Fe:messenger.exe main.cpp ^
/I Libraries\SFML-3.0.2\include ^
/link /LIBPATH:Libraries\SFML-3.0.2\lib ^
sfml-network.lib sfml-system.lib ws2_32.lib bthprops.lib user32.lib advapi32.lib
```
### You will see
A console application in which two modes can be selected:
1. SFML chat
2. Bluetooth chat
SFML chat can be tested on one device, just run two exe files on this device, where in the first file you are the client, and in the second you are the server.
If you have selected Bluetooth chat, then you need to test it on two devices, the main thing is to make sure that Bluetooth is enabled on them. On the client side, information about the port will be displayed, and on the server side, you must confirm the port you are connecting to.
