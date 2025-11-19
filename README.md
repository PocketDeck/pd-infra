# PocketDeck Infrastructure (`pd-infra`)

`pd-infra` contains the deployment infrastructure for **PocketDeck**, an online platform for playing multiplayer card games.
This repository provides a **single-container Docker-based deployment** that builds and serves all PocketDeck components behind an Nginx reverse proxy.

The container automatically fetches and builds the following public PocketDeck repositories:

- **`qr-gen`** — Small utility service used by the frontend to generate join-room QR codes.
- **`pd-core`** — Main backend server responsible for game logic, player sessions, and WebSocket communication.
- **`pd-web`** — Web frontend served via a lightweight Python application.

This repository does **not** perform any system configuration. It only builds these services inside a container and runs them using a unified startup process. Deployment, scaling, and hosting choices are left to the operator.

---

## Features

### **Single-Container Deployment**
All PocketDeck services run inside a single Docker container for simplified hosting:

- `qr-gen` built via `make`
- `pd-core` built using CMake
- `pd-web` launched as a Python app
- Nginx handles external access and routes the correct paths to each internal service

### **Reverse Proxy Routing**
Nginx distributes incoming traffic:

| Route        | Internal Service | Purpose |
|--------------|------------------|---------|
| `/qr`        | qr-gen           | QR code generation for joining rooms |
| `/ws`        | pd-core          | Backend WebSocket/game server |
| `/` (default) | pd-web           | Frontend application |

### **Automated Build Process**
The Dockerfile included in this repository:

- Pulls PocketDeck components from GitHub over HTTPS
- Installs required build tools (CMake, Make, compilers, Python, Nginx)
- Builds each service
- Configures Nginx
- Launches all components via a unified `start.sh` script

---

## Repository Structure

```
pd-infra/
│
├── Dockerfile # Builds the full PocketDeck environment
├── nginx.conf # Reverse proxy configuration (http-level include)
├── start.sh   # Supervises and launches all services
└── README.md  # This file
```

---

## Deployment

### **Build the container**
```bash
docker build -t pocketdeck .
```

### **Run the container**
```bash
docker run -p 80:80 pocketdeck
```

The PocketDeck system will be accessible on port 80, with all internal routing handled automatically by Nginx.

No additional environment variables or configuration are required.

---

## Notes
- This repository is intended **only for deployment**.
It does not provide development tools, local testing helpers, or environment setup.
- All PocketDeck components pulled during the build are public GitHub repositories, so no credentials are required.
- Operators are responsible for choosing where and how the container is hosted (VPS, cloud service, etc.).
