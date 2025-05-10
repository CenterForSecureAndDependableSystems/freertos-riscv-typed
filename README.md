# FreeRTOS Risc-V Simulator

TODO: Project Description

When cloning this repo, make sure you fetch all submodules as well:
```bash
git clone --recurse-submodules https://github.com/u1f98e/freertos-riscv-typed.git
```

More information on changes made to SPIKE can be found in [Status.md](Status.md)

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

### Building Crosstools (Optional, NOT RECOMMENDED)
The crosstools toolchain required for this project is provided as an image (see above).
It takes a long time, so generally it's recommended to use the prebuilt image instead.
If you need to update crosstools or adjust it's configuration though, you can build a
local image:

Adjust configuration within [crosstool/crosstool-defconfig](crosstool/crosstool-defconfig) or update the
submodule at [crosstool/crosstool-ng](crosstool/crosstool-ng), then:

```bash
cd crosstool
# Check the root Dockerfile to 
docker build . --tag u1f98e/crosstools-riscv-zicsr
```

Building crosstools takes a while (~30 minutes on my machine). Docker should
then use your local image instead of pulling it from Dockerhub, if it doesn't
you can replace the first `FROM` statement in the root [Dockerfile](Dockerfile) with
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
# First time:
docker run -it riscv-freertos --name sel

# Subsequent times:
docker start sel
docker exec -it sel /bin/bash
```

Generally for development, I work and commit changes from inside the container.
You can forward your ssh keys for commiting things to the git repo:
```bash
# On the host system, enable ssh agent forwarding. You will likely need to look
# up how to do this for your specific system, I can't test on windows :(

# For linux, enable AllowAgentForwarding in /etc/ssh/sshd_config and restart sshd.
# Then, start the agent:
eval "$(ssh-agent -s)"
# Confirm your keys are available:
ssh-add -l

# Then create the container, mounting the agent socket
docker run -it \
  --name sel \
  -v "$SSH_AUTH_SOCK:/ssh-agent:z" \
  -e SSH_AUTH_SOCK=/ssh-agent \
  riscv-freertos

# Within the container, check for keys:
ssh-add -l

# If that doesn't work, my last resort has been just copying my keys into the container.
```

If you need root access (i.e. for installing packages or reinstalling tools),
you can `exec` into the container as root and set up a sudo password for the
`researcher` user (there is none by default):

```bash
docker exec -it -u root sel /bin/bash
passwd researcher
```

From inside the container, you can run the simulator:
```bash
cd tests/basicTag
make
spike-run32 build/basicTag32.axf

# spike-run32 is a script which expands to:
spike \
	-p1 \
	--isa RV32IMA \
	-m0x80000000:0x10000000 \
	--tag-mem 0x90000000:0x10000000:0x80000000 \
	--rbb-port 9824 \
	$@
```

To test with type tags, you'll need to create a tag region.
Tags are stored in separate regions of memory which are mapped on top of normal memory regions created with `-m`.
A region of tag memory can be created and mapped using the `--tag-mem` flag:

```bash
# -m0x80000000:0x10000   
# Create a region of memory with base address 0x80000000 and size 0x10000
#
# --tag-mem 0x90000000:0x10000:0x80000000
# Create a region of tag memory at 0x90000000 of size 0x10000 mapped to 0x80000000.
$ spike -m0x80000000:0x10000 --tag-mem 0x90000000:0x10000:0x80000000 prog
```

Tag regions must completely overlap with a normal region (they can be smaller, 
but can't go past the beginning or end of the normal region). Address space with no tag region mapped to it
will ignore tag operations and should report back `0x0` if an instruction tries
to load a tag.

Other tests can be found in the `~/project/tests` directory. You
should be able to build the `.axf` files with `make`.

Check [Status.md](Status.md) for more details.
