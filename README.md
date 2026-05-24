# PocketDeck Infrastructure (`pd-infra`)

`pd-infra` provides the deployment infrastructure for **PocketDeck**, an online platform for playing multiplayer card games.

This repository builds and runs all PocketDeck components inside a **single Docker container** behind an Nginx reverse proxy. The components are pulled from their remote repositories at build time — no submodules are used.

---

## Components

| Component  | Source Repository         | Role |
|------------|---------------------------|------|
| `qr-gen`   | `qr-gen`                  | QR code generation for room invites (fastcgi) |
| `pd-core`  | `pd-core3`                | Backend server — game logic, WebSocket, player sessions  |
| `pd-web`   | `pd-web`                  | Web frontend served via a lightweight Python HTTP server |

---

## Features

- **Single-container deployment** — all services run inside one Docker image.
- **Build-time cloning** — each component is `git clone --depth 1` from its repository during the multi-stage build. No submodule maintenance required.
- **Nginx reverse proxy** — routes external traffic to the correct internal service.
- **No environment config needed** — the container runs out of the box on port 80.

### Route Mapping

| Route       | Upstream      | Purpose |
|-------------|---------------|---------|
| `/qr/`      | `qr-gen`      | QR code generation via fastcgi |
| `/ws/`      | `pd-core`     | WebSocket / game server on `:9001` |
| `/`         | `pd-web`      | Frontend served on `:9002` |

---

## Repository Structure

```
pd-infra/
├── Containerfile          # Multi-stage Docker build
├── nginx.conf             # Nginx reverse proxy configuration
├── scripts/
│   ├── start.sh           # Launches all services inside the container
│   └── qr-gen-http.sh     # FastCGI script for QR generation
├── .gitignore
└── README.md
```

---

## Usage

### Build

```bash
docker build -t pocketdeck -f Containerfile .
```

The build clones `qr-gen`, `pd-core3`, and `pd-web` from GitHub, builds `qr-gen` and `pd-core3`, and copies the web frontend.

### Run

```bash
docker run -p 80:80 pocketdeck
```

PocketDeck is now available at `http://localhost`. The `start.sh` script launches three processes:

- **pd-web** — Python HTTP server on `:9002`
- **pd-core** — Game server on `:9001`
- **nginx** — Reverse proxy on `:80`

Press Ctrl+C to stop all services.

---

## Notes

- This repository is for **deployment only** — it does not include development tooling or local setup helpers.
- All component repositories are **public** on GitHub.
- The default Git remote can be overridden with `--build-arg GIT_HOST=...`.
- Hosting, scaling, SSL, and server management are the operator's responsibility.
