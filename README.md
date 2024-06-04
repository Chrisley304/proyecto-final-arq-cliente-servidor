# Proyecto Final Arquitectura Cliente-Servidor 2024-2

El proyecto consiste en crear un Cliente-Servidor que ejecute comandos remotamente, como ocurre 
con  un  Cliente-Servidor  SSH  comercial  o  gratuito.

El  proyecto  debe  cumplir  con  los  siguientes 
requerimientos: 

- Estar desarrollado en una arquitectura cliente-servidor, con sockets TCP/IP y con conexión 
remota. Aunque los códigos los desarrolles en una misma máquina y lo pruebes localmente, 
pero la revisión será remota de host a host. 
- Debe estar desarrollado en un ambiente Linux, en la distribución que gustes o en MacOS y 
programado en lenguaje C.
- No se aceptan proyectos programados en otros lenguajes.


## ¿Cómo ejecutar el proyecto?

Para ejecutar el proyecto, primero se debe compilar el código fuente. Para ello, se debe ejecutar el siguiente comando:

```bash
gcc -o server server.c
gcc -o client client.c
```

Posteriormente, se debe ejecutar el servidor con el siguiente comando:

```bash
./server
```

Finalmente, se debe ejecutar el cliente con el siguiente comando:

```bash
./client <USUARIO>@<IP>:<PUERTO>
```

Donde `<USUARIO>` es el nombre de usuario del servidor, `<IP>` es la dirección IP del servidor y `<PUERTO>` es el puerto en el que el servidor está escuchando.

## Comandos soportados

### Servidor
El servidor soporta los siguientes comandos:
- `./server -h`: Muestra la ayuda del servidor.
- `./server <PUERTO>`: Establece el puerto en el que el servidor escuchará conexiones.
- `./server`: Inicia el servidor en el puerto por default 8080.

### Cliente
El cliente soporta los siguientes comandos:
- `./client -h`: Muestra la ayuda del cliente.
- `./client <USUARIO>@<IP>:<PUERTO>`: Establece la conexión con el servidor.

## Autores

- Christian Leyva
- Gerardo Martínez