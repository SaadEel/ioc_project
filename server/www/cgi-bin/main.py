#!/usr/bin/env python3
print("Content-Type: text/html\n")

html = """
<head>
  <title>Peri Web Server</title>
</head>
<body>
<h3>Commande LED</h3>
<form method="POST" action="led.py">
  <input name="val" cols="20"></input>
  <input type="submit" value="Entrer">
</form>

<hr>

<h3>Afficher un texte sur l'ecran</h3>
<form method="POST" action="ecran.py">
  <input name="message" cols="30"></input>
  <input type="submit" value="Envoyer texte">
</form>

<hr>

<h3>Consulter la luminosite</h3>
<form method="GET" action="photo.py">
  <input type="submit" value="Voir valeur lumiere">
</form>
</body>
"""

print(html)
