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