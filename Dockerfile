# Usar Debian 12 (Bookworm) como imagen base
FROM debian:bookworm-slim

# Evitar interacciones durante la instalación de paquetes
ENV DEBIAN_FRONTEND=noninteractive

# Instalar las dependencias necesarias
RUN apt-get update && apt-get install -y \
    build-essential \
    manpages-dev \
    openssh-server \
    net-tools \
    iproute2 \
    dnsutils \
    tcpdump \
    inetutils-ping \
    && rm -rf /var/lib/apt/lists/*


# Crear el directorio del proyecto
WORKDIR /app

# Copiar los archivos del proyecto
# COPY . .

# Dar permisos de ejecución al script de dependencias
# RUN chmod +x dependencies.sh

# Comando por defecto
CMD ["/bin/bash"] 