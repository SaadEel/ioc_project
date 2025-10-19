#!/usr/bin/env python3
import os
import paho.mqtt.client as mqtt
import sqlite3

s2f_path = "/tmp/s2f_fw"
f2s_path = "/tmp/f2s_fw"

# Créer les FIFO si elles n'existent pas
if not os.path.exists(s2f_path):
    os.mkfifo(s2f_path, 0o666)
if not os.path.exists(f2s_path):
    os.mkfifo(f2s_path, 0o666)

# Connexion à la base SQLite (auto-créée si absente)
db_path = os.path.abspath("mqtt_data.db")
conn = sqlite3.connect(db_path, check_same_thread=False)
cursor = conn.cursor()

# Création de la table de logs si elle n'existe pas
cursor.execute("""
    CREATE TABLE IF NOT EXISTS messages (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        topic TEXT,
        payload TEXT,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
    )
""")
conn.commit()

# Callback MQTT
def on_connect(client, userdata, flags, rc):
    print(f"[MQTT] Connecté (code={rc})")
    # On s'abonne pour capter les publications de l'ESP32
    client.subscribe("photo")
    client.subscribe("led")

def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    topic = msg.topic
    print(f"[MQTT] Reçu : {topic} → {payload}")

    # Stocker le message dans la base
    cursor.execute("INSERT INTO messages (topic, payload) VALUES (?, ?)", (topic, payload))
    conn.commit()

# Initialisation du client MQTT
mqttc = mqtt.Client()
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.connect("localhost", 1883, 60)
mqttc.loop_start()

print("[MQTT Gateway] En attente de commandes via FIFO...")

fifo = open(s2f_path, 'r')
# Boucle principale : lecture des commandes depuis la FIFO
while True:
    for ligne in fifo:
        ligne = ligne.strip()
        print(f"[FIFO] Reçu : {ligne}")

        if ligne.startswith("w "):
            commande = ligne[2:].strip()
            mqttc.publish("led", commande)
            print(f"[MQTT] Publication sur led : {commande}")
            with open(f2s_path, 'w') as f:
                f.write(f"Commande envoyée : {commande}\n")
        
        elif ligne.startswith("e "):
            texte = ligne[2:].strip()
            mqttc.publish("ecran", texte)
            print(f"[MQTT] Publication sur ecran : {texte}")
            with open(f2s_path, 'w') as f:
                f.write(f"Message envoyé : {texte}\n")
