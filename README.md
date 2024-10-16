# FreeRTOS Risc-V Simulator

## Setup
This repository provides a Dockerfile for building an image which can be used to create new containers.

You'll need an OCI compatible runtime like [Docker](https://docs.docker.com/engine/install/) or [Podman](https://podman.io/docs/installation).

You can either use a prebuilt image file or build a new image yourself, though building can take some time.

### Building an Image
In the repository directory (containing the `Dockerfile` and all other supplementary files and directories), run:

```bash
docker build . --tag riscv-freertos
```

You should now have a final image named `riscv-freertos`.

### Importing an Image
To import a prebuilt image file:

```bash
docker image import <image file>
```

You can also commit containers to save changes in them to new images, and save those images to files. See the documentation for more details:
- https://docs.docker.com/reference/cli/docker/container/commit/
- https://docs.docker.com/reference/cli/docker/image/save/

## Running the Container
Either use the Docker GUI to start and enter the container, or use:

```bash
docker run -it <image name or id>
```

From inside the container, you can run the simulator:
```bash
cd ~/FreeRTOS/FreeRTOS/Demo/RISC-V-spike-htif_GCC
spike -p1 --isa RV32IMA -m0x80000000:0x10000000 --rbb-port 9824 ./build/RTOSDemo32.axf

# Some convenience scripts can be found in the `scripts` directory
spike-run32 ./build/RTOSDemo32.axf
```

Other tests can be found in the `~/FreeRTOS/FreeRTOS/tests` directory.
