#!/usr/bin/env python3
import cgi

print("Content-Type: text/html\n")

form = cgi.FieldStorage()
text = form.getvalue('message')

s2fName = '/tmp/s2f_fw'
f2sName = '/tmp/f2s_fw'

try:
    with open(s2fName, 'w') as s2f:
        s2f.write(f"e {text}\n")
        s2f.flush()

    with open(f2sName, 'r') as f2s:
        res = f2s.readline().strip()
except Exception as e:
    res = f"Erreur : {e}"

html = f"""
<head>
  <title>Écran ESP32</title>
  <META HTTP-EQUIV="Refresh" CONTENT="2; URL=/cgi-bin/main.py">
</head>
<body>
Texte envoye a l'ecran : <b>{text}</b><br>
Resultat : {res}
</body>
"""

print(html)
