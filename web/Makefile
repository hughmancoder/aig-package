.PHONY: all dev build install wasm preview clean

all: dev

install:
	npm install

dev:
	@if command -v em++ >/dev/null 2>&1; then \
		echo "Building WebAssembly module..."; \
		../scripts/build_wasm.sh || true; \
	fi
	npm run dev

build:
	@if command -v em++ >/dev/null 2>&1; then \
		echo "Building WebAssembly module..."; \
		../scripts/build_wasm.sh || true; \
	fi
	npm run build

wasm:
	@../scripts/build_wasm.sh

preview:
	npm run preview

clean:
	rm -rf dist node_modules/.vite
