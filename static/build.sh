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
dat datos.dat -a ./bruno_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./bg*.tmx -h statics.h
dat datos.dat -a ./continue.tmx -h statics.h
dat datos.dat -a ./jordi_logo.bmp -h statics.h -t BMP
dat datos.dat -a ./palete_jordi_logo.bmp -h statics.h -t PAL
dat datos.dat -a ./msdosclub.bmp -h statics.h -t BMP
dat datos.dat -a ./intro.mid -h statics.h -t MIDI
dat datos.dat -a ./msdos.mid -h statics.h -t MIDI
#dat datos.dat -a ./stage_enemies.def -h statics.h 
dat datos.dat -a ./lifebar_bruno.bmp -h statics.h -t BMP
dat datos.dat -a ./lifebar_martin.bmp -h statics.h -t BMP
dat datos.dat -a ./lifebar.bmp -h statics.h -t BMP
dat datos.dat -a ./head.bmp -h statics.h -t BMP
dat datos.dat -a ./money.bmp -h statics.h -t BMP
dat datos.dat -a ./font.fnt -h statics.h
dat datos.dat -a ./bg1_shop2.bmp -h statics.h -t BMP
dat datos.dat -a ./level2_intro.bmp -h statics.h -t BMP
dat datos.dat -a ./level3_intro2.bmp -h statics.h -t BMP
dat datos.dat -a ./level3_intro3.bmp -h statics.h -t BMP
dat datos.dat -a ./level4_intro.bmp -h statics.h -t BMP
dat datos.dat -a ./level4_crash.bmp -h statics.h -t BMP
dat datos.dat -a ./farola.bmp -h statics.h -t BMP
dat datos.dat -a ./bomb_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./arrow_getin.bmp -h statics.h -t BMP
dat datos.dat -a ./jeringa.bmp -h statics.h -t BMP
dat datos.dat -a ./plataforma.bmp -h statics.h -t BMP
dat datos.dat -a ./plataforma2.bmp -h statics.h -t BMP
dat datos.dat -a ./pfuel.bmp -h statics.h -t BMP
dat datos.dat -a ./pcondenser.bmp -h statics.h -t BMP
dat datos.dat -a ./pflow.bmp -h statics.h -t BMP
dat datos.dat -a ./microphone.bmp -h statics.h -t BMP
dat datos.dat -a ./bullet.bmp -h statics.h -t BMP
dat datos.dat -a ./ushanka.bmp -h statics.h -t BMP
dat datos.dat -a ./rama.bmp -h statics.h -t BMP
dat datos.dat -a ./lifebar_throwable.bmp -h statics.h -t BMP
dat datos.dat -a ./lifebar_enemy.bmp -h statics.h -t BMP
dat datos.dat -a ./minicar.bmp -h statics.h -t BMP
dat datos.dat -a ./text_menu.bmp -h statics.h -t BMP
dat datos.dat -a ./sinking1.bmp -h statics.h -t BMP
dat datos.dat -a ./sinking2.bmp -h statics.h -t BMP
dat datos.dat -a ./tnt.bmp -h statics.h -t BMP
dat datos.dat -a ./end_level.bmp -h statics.h -t BMP
dat datos.dat -a ./big_spritesheet.bmp -h statics.h -t BMP
dat datos.dat -a ./guard_spritesheet.bmp -h statics.h -t BMP

dat intro.dat -a ./cars2.bmp -h intro_statics.h -t BMP

dat video.dat -a ./intro.fli -h video_statics.h 
dat video.dat -a ./intro2.fli -h video_statics.h 
dat video.dat -a ./intro_level41.fli -h video_statics.h 

dat -l datos.dat
cp statics.h /src
cp intro_statics.h /src
cp video_statics.h /src
