# Taller de métodos estocásticos 

    Repositorio:   https://github.com/alreraso/estocasticos

Este repositorio contiene la solucion del taller de la materia métodos estocásticos
dictada por el profesor Jorge Ortiz Triviño

En él se solucionan los siguientes puntos:

a.    Para el taller debe realizar una red ad hoc con 20 nodos se puede usar el código de mixed -wireless- ad hoc que se encuentra en la carpeta wireless de los ejemplos.

b.    En este escenario se deben usar diversos tipos de tráfico y cambiando su distribución.

c.     Una vez hecho está simulación realizar un escenario con 20 nodos con el módulo LoRaWAN instalado con los mismos tipos de tráfico usados en la simulación anterior. Para ello debe modificar algunos de estos tres programas (simple-network-example, complete-network-example o network-server-example).

d.    Evalúe con la herramienta OpenAI Gym cómo optimizar el desempeño del sistema basado en LoRaWAN (Las medidas de desempeño a considerar las determina el grupo desarrollador con base en criterios técnicos comúnmente usados en redes)

# Requerimientos 📋

* Sistema operativo Ubuntu (se recomienda la versión 20.04)
* Tener instalado NS3
* Tener instalado OpenAI Gym
    * De no contar con ns3 y opengym puede encontrar en la carpeta de ns3 opengym la documentacion de instalacion.

* Tener instalado el módulo de LoRaWan
    * De no tenerlo puede seguir el instructivo de la pagina del modulo https://apps.nsnam.org/app/lorawan/ buscando el link de code repository, este lo redireccionada a link https://github.com/signetlabdei/lorawan ahi podra encontrar las intrucciones de instalacion.

**En caso de no saber como instalar NS3 OpenAi Gym o LoRaWan se recomienda pedir en un pull request las instrucciones

# Consideraciones 🔧
Los archivos contienen comentarios con respecto a la ejecucion de ciertas versiones de ns3 y a los diferentes traficos o modificaciones
de topologia (distribucion de la red), en caso de tener algun fallo, se recomienda leer los comentarios de ciertas lineas que le permitiran ejecutar correctamente los archivos, en algunso casos las versiones de ns3 instaladas difieren y ahi se da la solucion.


# Despliegue 🚀

Una vez cuente con los requerimientos completamente podra ejecutar los escenarios de la siguiente manera, para cada escenario debe copiar la carpeta del escenario que desea ejecutar en la carpeta scratch que encontraradentro de ns3, luego de ello podra ejecutar el comando de ejecucion dependiendo del escenario como se especifica adelante:


 # Escenario 1 NS3 🔩
```sudo ./waf --run redNs3```  

Este comando debe hacerce un nivel arriba de la carpeta scratch.


 # Escenario 2 red LoRaWan 🔩

```sudo ./waf --run redLorawan```

Este comando debe hacerce un nivel arriba de la carpeta scratch.

# Escenario 3 red con openAi Gym 🔩

Para este escenario deberá tener dos terminales:

La primera terminal deberá ejecutar el comando sudo
```./waf --run redOpenGym```
Este comando debe hacerce un nivel arriba de la carpeta scratch, cuando aparezca el mensaje de ejecutar el agente use la segunda terminal.

La segunda terminal debe navegar hasta el archivo .py de la carpeta de redOpenGym,
Los comandos de navegación son:

```cd scratch``` 

Luego debera usar 


   
```cd redOpenGym``` 
    

    
    
Una vez dentro de la carpeta redOpenGym podra ejecutar el comando: 


```python3 agentes.py```



# Documentación anexa 📌

Encontrara tambien un poco mas de informacion sobre redes ad hoc, la descripcion de los ficheros en la carpeta de documentacion de taller
