#make
rm -r ./kmsshooter_game/*
cp kmsshooter ./kmsshooter_game/
cp editme.conf ./kmsshooter_game/
cp -r ./shaders ./kmsshooter_game/
tar -c ./kmsshooter_game/ -f kmsshooter_game.tar
