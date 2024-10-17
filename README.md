# FreeRTOS Risc-V Simulator

# Setup
This repository provides a Dockerfile for building an image which can be used
to create new containers. You'll need an OCI compatible runtime like 
[Docker](https://docs.docker.com/engine/install/) or 
[Podman](https://podman.io/docs/installation).
You can either use a prebuilt image file or build a new image yourself, though
building can take some time.

## Building the Images
Two images are present in this repo, one for building the
`crosstools-riscv-zicsr` dependency, and the main one. `crosstools-riscv-zicsr`
is provided on DockerHub [here](https://hub.docker.com/repository/docker/u1f98e/crosstools-riscv32-zicsr/general), 
but you can also build it locally if needed.

### Building Crosstools (Optional)
If you need to update crosstools or adjust it's configuration, you can build a
local image:

Adjust configuration within [crosstool/crosstool-defconfig] or update the
submodule at [crosstool/crosstool-ng], then:

```bash
cd crosstool
# Check the root Dockerfile to 
docker build . --tag u1f98e/crosstools-riscv-zicsr
```

Building crosstools takes a while (~30 minutes on my machine). Docker should
then use your local image instead of pulling it from Dockerhub, if it doesn't
you can replace the first `FROM` statement in the root [Dockerfile] with
`localhost/crosstools-riscv-zicsr`.

### Build the Simulator/FreeRTOS Image
In the repository root, run:

```bash
docker build . --tag riscv-freertos
```

You should now have a final image named `riscv-freertos`.

## Running the Container
Either use the Docker GUI to start and enter the container, or use:

```bash
docker run -it riscv-freertos
```

From inside the container, you can run the simulator:
```bash
cd ~/FreeRTOS/FreeRTOS/Demo/RISC-V-spike-htif_GCC
spike-run32 ./build/RTOSDemo32.axf

# spike-run32 is a script which expands to:
spike -p1 --isa RV32IMA -m0x80000000:0x10000000 --rbb-port 9824 ./build/RTOSDemo32.axf
```

Other tests can be found in the `~/FreeRTOS/FreeRTOS/tests` directory. You
should be able to build the `.axf` files with `make`.
