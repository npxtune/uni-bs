# Repo für BS

### MacOS:
Setup:
```
./setup/setup.sh
```

Danach in CLion die Dockerimage als Toolchain mit den Container Settings:

`--entrypoint -p 127.0.0.1:5678:5678 --rm` 

hinzufügen, um den Server automatisch beim kompilieren des Projekts zu starten.

Um sich zu verbinden, einfach `./client.sh` ausführen! :)