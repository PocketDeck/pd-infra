# PocketDeck Infrastructure (`pd-infra`)

`pd-infra` contains the deployment infrastructure for **PocketDeck**, an online platform for playing multiplayer card games.
This repository provides a **single-container Docker-based deployment** that builds and serves all PocketDeck components behind an Nginx reverse proxy.

All PocketDeck components are included as **Git submodules** and are built directly inside the container during the Docker build process. This repository does **not** perform any machine-level setup; it only provides the build and runtime environment for deployment.

---

## Included Components (via Submodules)

The following PocketDeck repositories are included under `PocketDeck/` as Git submodules:

- **`qr-gen`** — QR code generation service used by the frontend to allow players to easily join rooms via shareable QR codes.
- **`pd-core`** — Main backend server responsible for game logic, player sessions, and WebSocket communication.
- **`pd-web`** — Web frontend served through a lightweight Python application.

Submodules ensure that deployment always uses specific, version-tracked revisions of each component.

---

## Features

### **Single-Container Deployment**
All PocketDeck services run inside one Docker container:

- `qr-gen` is built using `make`.
- `pd-core` is built using CMake.
- `pd-web` is launched as a Python application.
- Nginx serves as the reverse proxy and entrypoint for all external traffic.

### **Reverse Proxy Routing**
Nginx handles routing between all internal services:

| Route         | Internal Service | Purpose |
|---------------|------------------|---------|
| `/qr`         | qr-gen           | QR code generation for room invites |
| `/ws`         | pd-core          | Backend WebSocket/game logic |
| `/` (default) | pd-web           | Frontend application |

### **Automated Build Process**
The Dockerfile included in this repository:

- Copies the repository and its submodules into the container
- Installs required build dependencies (Make, CMake, Python, Nginx, etc.)
- Builds each component
- Configures Nginx
- Starts all services using a unified `start.sh` supervisor script

---

## Repository Structure

```
pd-infra/
│
├── PocketDeck/ # Submodule directory
│   ├── qr-gen/ # Submodule: QR generator
│   ├── pd-core/ # Submodule: Backend server
│   └── pd-web/ # Submodule: Frontend
│
├── Dockerfile # Builds the entire PocketDeck environment
├── nginx.conf # Reverse proxy configuration (http include)
├── start.sh # Launches all services inside the container
└── README.md # This file
```

---

## Working with Submodules

To clone this repository with all components included:

```bash
git clone --recurse-submodules https://github.com/PocketDeck/pd-infra.git
```

If the repository was already cloned without submodules:

```bash
git submodule update --init --recursive
```

To update all submodules to their latest remote commits:

```bash
git submodule update --remote --merge
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

No environment variables or additional configuration are required.

---

## Notes
- This repository is strictly for **deployment**; it does not provide development tools or local environment setup.
It does not provide development tools, local testing helpers, or environment setup.
- All PocketDeck components used as submodules are **public GitHub repositories**.
- Hosting, scaling, SSL termination, and server management are fully up to the operator.
