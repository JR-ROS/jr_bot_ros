# Integrated Brain ROS Workspace

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

## Quickstart guide for running Docker containers manually

If the editor prompts you to "Reopen in Container", simply do so. When prompted, choose between "SBC" or "Host".