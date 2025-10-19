#!/usr/bin/env python3
print("Content-Type: text/html\n")

import sqlite3
import os

db_path = os.path.abspath("/home/seh22/M1_SESI/ioc/projet/mqtt_data.db")

try:
    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("SELECT payload, timestamp FROM messages WHERE topic='photo' ORDER BY id DESC LIMIT 1")
    row = cursor.fetchone()
    if row:
        val, ts = row
        message = f"Derniere luminosite recue : <b>{val}</b> à {ts}"
    else:
        message = "Aucune donnee disponible."
    conn.close()
except Exception as e:
    message = f"Erreur : {e}"

html = f"""
<head><title>Valeur luminosité</title>
<META HTTP-EQUIV="Refresh" CONTENT="2; URL=/cgi-bin/main.py">
</head>
<body>
{message}
</body>
"""

print(html)
