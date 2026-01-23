dat datos.dat -a ./pieldetoro_intro.bmp -h statics.h -t BMP
dat datos.dat -a ./menu2.bmp -h statics.h -t BMP
dat datos.dat -a ./tiles.bmp -h statics.h -t BMP
dat datos.dat -a ./coche_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./martin_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./bird_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./level2_intro.bmp -h statics.h -t BMP
dat datos.dat -a ./bg*.tmx -h statics.h

dat -l datos.dat
cp statics.h /src
