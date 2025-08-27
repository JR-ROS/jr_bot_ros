# Scripts for handling the development environment

The Docker environments used in this repository are hosted and built separately. Please see the documentation at their repository: https://github.com/eccentricOrange/int-brain-containers/

## Quickstart guide for running Docker containers manually

1.  Pull the base image and build the container.

    ```bash
    ./config/scripts/devel.sh TARGET
    ```

    Replace `TARGET` with the desired target (`sbc` or `host`).


2.  Enter the container.

    ```bash
    ./config/scripts/enter.sh TARGET
    ```

    Replace `TARGET` with the desired target (`sbc` or `host`).


3.  When you are done, stop the container.

    ```bash
    ./config/scripts/stop.sh
    ```

## Functions of each script

| Script | Arguments | Description | Used by Devcontainer |
| --- | --- | --- | --- |
| [devel.sh](devel.sh) | TARGET (`host` or `sbc`) | Uses images from `int-brain-containers` repository to create and start a Docker container | No |
| [enter_bash.sh](enter_bash.sh) | TARGET (`host` or `sbc`)  | Runs the image created by `devel.sh` | No |
| [stop.sh](stop.sh) | - | Stop and delete the container created by `devel.sh` | No |
| [convenience_functions.sh](convenience_functions.sh) | - | Create bash aliases for building and sourcing the ROS2 environment. | Yes |
| [post_create.sh](post_create.sh) | - | Configure the workspace and source `convenience_functions.sh` once the container run for the first time. | Yes |