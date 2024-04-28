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
./client
```

## Autores

- Christian Leyva
- Gerardo 