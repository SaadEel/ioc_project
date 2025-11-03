L'objectif du projet est un système reposant sur une carte ESP32 connectée à un 
serveur web hébergé sur une Raspberry Pi 3. La communication entre ces deux entités 
s’appuie sur le protocole MQTT, qui permet un échange efficace de messages entre 
différents éléments du système (capteurs, actionneurs, serveur, interface web). 

Le système se compose de plusieurs sous-ensembles : 
La carte ESP32, équipée :
- d’un capteur de luminosité (photorésistance), -
- de deux boutons-poussoirs (un intégré à la carte, un externe), 
- d’un écran (affichage de texte ou d'informations), 
- d’une LED (activable à distance), 
- éventuellement d’un buzzer. 

La Raspberry Pi 3, jouant le rôle de serveur central, héberge :
- un serveur HTTP accessible depuis un navigateur web,
- une gateway MQTT assurant l'interface entre les requêtes web et les messages, 
- MQTT, 
- un broker MQTT (routeur de messages), 
- une base de données enregistrant les valeurs issues des capteurs.
 
L’interface utilisateur est une simple page web contenant des boutons ou formulaires 
permettant d’interagir avec la carte ESP32 en temps réel. 
Ainsi, l’utilisateur peut, à distance, commander des actions sur la carte (comme allumer 
une LED ou écrire un texte sur l’écran), tout en visualisant les données provenant des 
capteurs connectés à l’ESP32. 


Pour tester la preuve de concept : à faire dans l'ordre

Lancer le broker :
mosquitto

Lancer la passerelle mqtt : (dans le répertoire server)
python3 server/mqtt_gateway.py

Lancer le serveur HTTP :
(Important : se placer avant dans le répertoire server/www)
python3 ../server.py

Lancer un client mqtt : (Dans le répertoire preuve_concept)
python3 client.py (Pour écouter les commandes)
python3 client_capteurs.py (Pour simuler des données qu'envoient l'esp32)

Chaque commande est à lancer dans un terminal différent

Pour consulter la base de données :
sqlite3 mqtt_data.db (qui sera automatiquement crée par mqtt_gateway.py)

Pour voir toutes les données enregistrées :
SELECT * FROM messages;

Pour tester le projet une fois sur la Raspberry :

Lancer la passerelle mqtt :
python3 server/mqtt_gateway.py

Lancer le serveur HTTP :
(Important : se placer avant dans le répertoire server/www)
python3 ../server.py


Et charger le code sur l'ESP32 via Arduino
