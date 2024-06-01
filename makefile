all: build_src
	@echo "\033[32m\tAuction System has been compiled successfully!\033[0m"

MAKEFLAGS += --no-print-directory

# Build source folder
build_src:
	@echo "Building sources..."
	@cd source && make --always-make

clean:
	@echo "Cleaning..."
	@cd source && make clean