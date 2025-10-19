import paho.mqtt.client as mqtt
import time

client = mqtt.Client()
client.connect("localhost", 1883, 60)

compteur = 0
try:
    while True:
        message = f"{compteur}"
        client.publish("photo", message)
        print(f"Message envoyé : {message}")
        compteur += 1
        time.sleep(1)  # toutes les secondes
except KeyboardInterrupt:
    print("Arrêt du simulateur.")
