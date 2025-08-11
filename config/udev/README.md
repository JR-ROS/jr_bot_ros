# `udev` Setup for Integrated Brain Device

These steps give you reliable, non-root access to your Integrated Brain ("int-brain") device on Linux (Ubuntu and others), using standard udev rules.

## Setup Steps

1. **Copy the udev rule**
   ```sh
   sudo cp config/udev/99-int-brain.rules /etc/udev/rules.d/
   ```

2. **Reload udev rules**
   ```sh
   sudo udevadm control --reload-rules
   sudo udevadm trigger
   ```

3. **Add your user to the `dialout` group**  
   (This gives permission to access serial devices.)
   ```sh
   sudo usermod -aG dialout $USER
   ```
   > [!NOTE]
   > You must log out and back in for group changes to take effect.

4. **Unplug and replug your int-brain device**

5. **Verify device symlinks**
   ```sh
   ls /dev/int-brain-*
   ```
   You should see something like `/dev/int-brain-12345678`.

6. **Use the symlink in your software**  
   (e.g., `/dev/int-brain-12345678`)

   > [!IMPORTANT]
   > Make sure that you always edit this symlink in your ROS2 or custom robotics software instead of the raw device path (like `/dev/ttyACM0`), especially if you have multiple devices connected.

## Troubleshooting

- Ensure `idVendor` and `idProduct` in `99-int-brain.rules` match your device (the example is for STM32 Virtual COM Port).
- Double-check that your user is in the `dialout` group. After running step 3, log out and in again.
- Try replugging the hardware after setup.
- If the symlink (`/dev/int-brain-*`) does not show, reboot the machine.
- Run this command to see permission and group info:
  ```sh
  ls -l /dev/int-brain-*
  ```
- For advanced debug, use:
  ```sh
  udevadm info -a -n /dev/ttyACM0
  ```