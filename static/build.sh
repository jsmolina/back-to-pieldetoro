rm datos.dat
dat datos.dat -a ./pieldetoro_intro3.bmp -h statics.h -t BMP
dat datos.dat -a ./palete_intro.bmp -h statics.h -t PAL
dat datos.dat -a ./menu2.bmp -h statics.h -t BMP
dat datos.dat -a ./book.bmp -h statics.h -t BMP
dat datos.dat -a ./tiles.bmp -h statics.h -t BMP
dat datos.dat -a ./coche_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./martin_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./bird_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./dog_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./joven_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./level2_intro.bmp -h statics.h -t BMP
dat datos.dat -a ./bg*.tmx -h statics.h
dat datos.dat -a ./jordi_logo.bmp -h statics.h -t BMP
dat datos.dat -a ./palete_jordi_logo.bmp -h statics.h -t PAL
dat datos.dat -a ./msdosclub.bmp -h statics.h -t BMP
dat datos.dat -a ./intro.mid -h statics.h -t MIDI
dat datos.dat -a ./msdos.mid -h statics.h -t MIDI
dat datos.dat -a ./stage_enemies.def -h statics.h 
dat datos.dat -a ./intro.fli -h statics.h 
dat datos.dat -a ./intro2.fli -h statics.h 
dat datos.dat -a ./lifebar_bruno.bmp -h statics.h -t BMP
dat datos.dat -a ./lifebar_martin.bmp -h statics.h -t BMP
dat datos.dat -a ./lifebar.bmp -h statics.h -t BMP
dat datos.dat -a ./head.bmp -h statics.h -t BMP
dat datos.dat -a ./numbers.bmp -h statics.h -t BMP
dat datos.dat -a ./money.bmp -h statics.h -t BMP


dat -l datos.dat
cp statics.h /src
