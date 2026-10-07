"""Serve this preview locally with Python 3; no third-party packages required.

    python WebPreview/serve.py
    python WebPreview/serve.py --port 9000 --open

The server always binds to 127.0.0.1 and serves the directory containing this file.
Press Ctrl+C in its terminal to stop it.
"""

import argparse
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import sys
import webbrowser


ROOT = Path(__file__).resolve().parent
HOST = "127.0.0.1"


class PreviewHandler(SimpleHTTPRequestHandler):
    extensions_map = {
        **SimpleHTTPRequestHandler.extensions_map,
        ".js": "text/javascript; charset=utf-8",
        ".json": "application/json; charset=utf-8",
        ".css": "text/css; charset=utf-8",
        ".svg": "image/svg+xml",
        ".mp4": "video/mp4",
        ".webm": "video/webm",
        ".wav": "audio/wav",
        ".ogg": "audio/ogg",
    }

    def end_headers(self):
        self.send_header("Cache-Control", "no-cache")
        super().end_headers()


def port_number(value):
    try:
        port = int(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("port must be an integer") from exc
    if not 1 <= port <= 65535:
        raise argparse.ArgumentTypeError("port must be between 1 and 65535")
    return port


def main():
    parser = argparse.ArgumentParser(description="Serve the UniversalMenu web preview on 127.0.0.1.")
    parser.add_argument("--port", "-p", type=port_number, default=8765, help="local port (default: 8765)")
    parser.add_argument("--open", action="store_true", dest="open_browser", help="open the preview in your default browser")
    args = parser.parse_args()
    handler = partial(PreviewHandler, directory=str(ROOT))
    try:
        server = ThreadingHTTPServer((HOST, args.port), handler)
    except OSError as exc:
        print(f"Cannot start preview on {HOST}:{args.port}: {exc}", file=sys.stderr)
        print("If the port is in use, choose another with --port 9000.", file=sys.stderr)
        return 1

    url = f"http://{HOST}:{args.port}/"
    print(f"UniversalMenu preview: {url}", flush=True)
    print(f"Serving: {ROOT}\nPress Ctrl+C to stop.", flush=True)
    if args.open_browser:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nPreview stopped.", flush=True)
    finally:
        server.server_close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
