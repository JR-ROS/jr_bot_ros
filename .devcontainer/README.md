# Integrated Brain Container System

## Overview
- It is expected that you run your robot in a [headless mode](https://en.wikipedia.org/wiki/Headless_computer), and handle any visualization on your local machine using ROS' communication system.
- Separate Dockerfiles are provided for the SBC/server (called "sbc") and the local machine (called "host").
- You may either run the Docker containers directly from a command line, or use them as [Dev Containers](https://containers.dev/).
- Usage steps are very similar for both systems.

## Running using Docker directly

1.  **Finish any local setup**

    Make sure that any necessary networking configuration (e.g., connecting to a Wi-Fi network), and hardware setup (e.g., adding relevant Udev rules) is completed on your system.

2.  **Build the Docker image**

    Simply run the build script from the root of the repository:

    ```bash
    ./scripts/build_img.sh
    ```

3.  **Update the run file**

    [`devel.sh`](/scripts/devel.sh) provides arguments to connect your host machine's display to the container, and expose the serial port of your robot. You may need to update the serial port path to match your system.

    Four versions of the command are provided in the script, depending on whether you are running on the host machine or a remote machine, and whether the robot is connected or not. You will need to uncomment the appropriate command based on your setup.

4.  **Run the container**

    Run the script to start the container:

    ```bash
    ./scripts/devel.sh
    ```

5.  **Enter the container**

    This script is also provided in two versions, depending on whether you are running on the host machine or a remote machine. Select the appropriate command by uncommenting it in the script, and run it:

    ```bash
    ./scripts/enter_bash.sh
    ```

6.  **Get dependencies and build the workspace**

    Once inside the container, you will need to run the [`post_create.sh`](/scripts/post_create.sh) script to install dependencies and build the workspace:

    ```bash
    ./scripts/post_create.sh
    ```

    This is only necessary the first time you run the container, or if you have made changes to the workspace that required rebuilding.


## Running using Dev Containers

1.  **Finish any local setup**

    Make sure that any necessary networking configuration (e.g., connecting to a Wi-Fi network), and hardware setup (e.g., adding relevant Udev rules) is completed on your system.

2.  **Update the Dev Container configuration**

    You may need to update the serial port path to match your system in your Devcontainer configuration file.

3.  **Open the Dev Container**

    If you are using Visual Studio Code, you can open the Dev Container by clicking on the green button in the bottom left corner of the window and selecting "Reopen in Container". Simply select the "sbc" or "host" configuration depending on your setup, and Visual Studio Code will build the container and open it for you.

    The [`post_create.sh`](/scripts/post_create.sh) script will be run automatically after the container is created, installing dependencies and building the workspace.

## Convenience functions
A couple of convenience functions are provided, implemented as aliases in `~/.bashrc`, if you use the post-create script to set up your environment.

-   **`wss` Source Workspace**

    This function sources the workspace setup file.

    It is equivalent to running the following command:

    ```bash
    source /workspaces/install/setup.bash && echo "Sourced workspace"
    ```

-   **`wsb` Build Workspace**

    This function builds the workspace and then sources it.

    It is equivalent to running the following command:

    ```bash
    cd /workspaces && colcon build --symlink-install && wss
    ```


## Acknowledgements
The Devcontainer system of this project was inspired by the Wheelchair project at [RRC, IIIT Hyderabad](https://github.com/Smart-Wheelchair-RRC/). Please see their project [DockerForDevelopment](https://github.com/Smart-Wheelchair-RRC/DockerForDevelopment).