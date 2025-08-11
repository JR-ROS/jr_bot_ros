FROM ghcr.io/eccentricOrange/int_brain_common:dev0.1

## install hardware-specific packages
RUN apt-get install --no-install-recommends -y \
    python3-lgpio \
    python3-gpiozero \
    python3-smbus \
    i2c-tools