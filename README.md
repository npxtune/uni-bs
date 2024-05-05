# Repo für BS

### MacOS:
Setup:
```
./setup/setup.sh
```

Danach in CLion die Dockerimage als Toolchain mit den Container Settings:

`--entrypoint -p 127.0.0.1:5678:5678 --rm` 

![Screenshot 2024-05-05 at 14 36 19](https://github.com/npxtune/uni-bs/assets/42376598/9fafd671-34a9-42ac-a7f4-9f6f5f83a64c)


hinzufügen, um den Server automatisch beim kompilieren des Projekts zu starten.

**WICHTIG!** Bitte Docker bei CMake als Toolchain auswählen!!!

![Screenshot 2024-05-05 at 14 22 00](https://github.com/npxtune/uni-bs/assets/42376598/1991e585-aa86-4a6c-833f-8038e68b89e5)

Um sich zu verbinden, einfach `./client.sh` ausführen! :)
