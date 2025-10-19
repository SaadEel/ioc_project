#!/usr/bin/env python3
import cgi, os, time,sys

print("Content-Type: text/html\n")

form = cgi.FieldStorage()
val = form.getvalue('val')

s2fName = '/tmp/s2f_fw'
f2sName = '/tmp/f2s_fw'
try:
    # Ouverture non bloquante : lecture+écriture pour éviter le blocage FIFO
    s2f = open(s2fName, 'w')
    s2f.write(f"w {val}\n")
    s2f.flush()
    s2f.close()

    f2s = open(f2sName, 'r')
    res = f2s.readline().strip()
    f2s.close()
except Exception as e:
    res = f"Erreur : {e}"

html="""
<head>
  <title>Peri Web Server</title>
  <META HTTP-EQUIV="Refresh" CONTENT="1; URL=/cgi-bin/main.py">
</head>
<body>
LEDS:<br/>
<form method="POST" action="led.py">
  <input name="val" cols="20"></input>
  <input type="submit" value="Entrer">
  set %s
</form>
</body>
""" % (val,)

print(html)
