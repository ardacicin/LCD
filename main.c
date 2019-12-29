#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "Lcd.h"

/**
 * Portların Bağlantı şekilleri şu şekildedir:
 *
 *  Tiva C   LCD Port
 *   PB0  ->   RS
 *   PB1  ->   E
 *   PB4  ->   D4
 *   PB5  ->   D5
 *   PB6  ->   D6
 *   PB7  ->   D7
 */


void Lcd_init() {	  //Lcd başlangıç ayarları

        SysCtlPeripheralEnable(LCDPORTENABLE);
        GPIOPinTypeGPIOOutput(LCDPORT, 0xFF);

        SysCtlDelay(50000);

        GPIOPinWrite(LCDPORT, RS,  0x00 );
        //LCDPORT=GPIO_PORTB_BASE, RS=Register select, 0b0000000

        GPIOPinWrite(LCDPORT, D4 | D5 | D6 | D7,  0x30 ); // 0b0011 0000
        GPIOPinWrite(LCDPORT, E, 0x02); //E=Enable , 0b0000 0010
        SysCtlDelay(10); //Enable için delay koyuyoruz
        GPIOPinWrite(LCDPORT, E, 0x00);

        SysCtlDelay(50000);

        GPIOPinWrite(LCDPORT, D4 | D5 | D6 | D7,  0x30 );
        GPIOPinWrite(LCDPORT, E, 0x02);
        SysCtlDelay(10);
        GPIOPinWrite(LCDPORT, E, 0x00);

        SysCtlDelay(50000);

        GPIOPinWrite(LCDPORT, D4 | D5 | D6 | D7,  0x30 );
        GPIOPinWrite(LCDPORT, E, 0x02);
        SysCtlDelay(10);
        GPIOPinWrite(LCDPORT, E, 0x00);

        SysCtlDelay(50000);

        GPIOPinWrite(LCDPORT, D4 | D5 | D6 | D7,  0x20 ); //0b0010 0000
        GPIOPinWrite(LCDPORT, E, 0x02);
        SysCtlDelay(10);
        GPIOPinWrite(LCDPORT, E, 0x00);

        SysCtlDelay(50000);

        Lcd_Komut(0x28);	//0b0010 1000   3,5
        Lcd_Komut(0xC0);	//0b1100 0000   6,7
        Lcd_Komut(0x06);	//0b0000 0110	1,2
        Lcd_Komut(0x80);	//0b1000 0000	7
        Lcd_Komut(0x28);	//0b0010 1000	3,5
        Lcd_Komut(0x0f);	//0b0000 1111	0,1,2,3
        Lcd_Temizle();

}
//Lcd aktifleştirme işlemlerinde kaynak olarak
//http://www.mcu-turkey.com/stellaris-launchpad-16x2-lcd/
//kullandım
void Lcd_Komut(unsigned char c) {	//Lcd ye komut göndermeye yarar

        GPIOPinWrite(LCDPORT, D4 | D5 | D6 | D7, (c & 0xf0) ); //0b1111 0000
        GPIOPinWrite(LCDPORT, RS, 0x00);
        GPIOPinWrite(LCDPORT, E, 0x02);
        SysCtlDelay(50000);
        GPIOPinWrite(LCDPORT, E, 0x00);	//Disable yapıyoruz

        SysCtlDelay(50000);

        GPIOPinWrite(LCDPORT, D4 | D5 | D6 | D7, (c & 0x0f) << 4 ); //0b0000 1111
        GPIOPinWrite(LCDPORT, RS, 0x00);
        GPIOPinWrite(LCDPORT, E, 0x02);
        SysCtlDelay(10);
        GPIOPinWrite(LCDPORT, E, 0x00);

        SysCtlDelay(50000);

}

void Lcd_Putch(unsigned char d) {	//Tek karekter yazdırır

        GPIOPinWrite(LCDPORT, D4 | D5 | D6 | D7, (d & 0xf0) );
        GPIOPinWrite(LCDPORT, RS, 0x01);
        GPIOPinWrite(LCDPORT, E, 0x02);
        SysCtlDelay(10);
        GPIOPinWrite(LCDPORT, E, 0x00);

        SysCtlDelay(50000);

        GPIOPinWrite(LCDPORT, D4 | D5 | D6 | D7, (d & 0x0f) << 4 );
        GPIOPinWrite(LCDPORT, RS, 0x01);
        GPIOPinWrite(LCDPORT, E, 0x02);
        SysCtlDelay(10);
        GPIOPinWrite(LCDPORT, E, 0x00);

        SysCtlDelay(50000);

}
void imlec(char x, char y){	//Kursorü istenilen yere gönderir

        if(x==1)
                Lcd_Komut(0x80+((y-1)%16));	//1000 0000
        else
                Lcd_Komut(0xC0+((y-1)%16)); //1100 0000
}

void Lcd_Temizle(void){	 //Lcd ekranını temizler
        Lcd_Komut(0x01);
        SysCtlDelay(10);
}

void Lcd_Yaz( char* s){	//String ifade yazdırır
        while(*s)
                Lcd_Putch(*s++);
}

//http://yapbenzet.kocaeli.edu.tr/sistem-programlama-5-hafta-dokuman/#_porte_ye_d_ardan_buton_ba_lama
//Kaynağından alınmıştır.
void init_port_E() {
   volatile unsigned long tmp; // bu degisken gecikme yapmak icin gerekli
   SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;   // 1) E portunun osilatörünü etkinleştir
   tmp = SYSCTL_RCGCGPIO_R;    	// allow time for clock to start
   GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port E
   GPIO_PORTE_CR_R = 0x3F;         // allow changes to PE5-0 //PE5-0 değişikliklerine izin ver
                                   // only PE0 needs to be unlocked, other bits can't be locked
    			 // Sadece PE0 kilidinin açılması gerekir, diğer bitler kilitlenemez
   GPIO_PORTE_AMSEL_R = 0x00;    	// 3) disable analog on PE //PE'de analog devre dışı bırak
   GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PE4-0
   GPIO_PORTE_DIR_R = 0x0F;      	// 5) PE4,PE5 in, PE3-0 out
   GPIO_PORTE_AFSEL_R = 0x00;    	// 6) disable alt funct on PE7-0
   GPIO_PORTE_PUR_R = 0x30;      	// enable pull-up on PE5 and PE4
   	   	   	   	   	 //PE4 ve PE5'te pull up'ı etkinleştir ( BUTON İÇİN)
   GPIO_PORTE_DEN_R = 0x3F;      	// 7) enable digital I/O on PE5-0 // portE 5-0 giriş çıkış  etkinlerştir.
   }


int main(void) {

	volatile unsigned int delay;

	init_port_E();
	int kosul;
	int kosul2;
	int i;

	 SysCtlClockSet( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	 Lcd_init();
	 /**
	  * Program sonsuz döngüyle çalışıyor
	  * kosul dediğimiz şey 0-3 arası üretilen random bir sayıdır
	  * kosul2 random üretme işleminde bir önceki random sayıdır.
	  * bu sayılar bize hangi ledin yandığını yani hangi işlemlerin gerçekleşeceğini
	  * anlamamızda yardımcı olur.
	  */


	while(1){
		kosul=rand()%4;
		if(kosul!=kosul2){    //Bu kontrol aynı ledin 2 kez üst üste yanmamasını sağlıyor.
		if(kosul==0){		//Kırmızı led çalışıyor
			kosul2=0;
			GPIO_PORTE_DATA_R |= 0b00000001;	//İşimize yarayan ledi yakıp diğer ledleri
			GPIO_PORTE_DATA_R &= ~0b00000010;	//söndürdük
			GPIO_PORTE_DATA_R &= ~0b00000100;
			GPIO_PORTE_DATA_R &= ~0b00001000;
		    imlec(1,3);				//2x16 lık lcd de 1. satır 3. sutundan yazdırmaya başıyoruz.
		    Lcd_Yaz("Kocaeli Uni");
		    for(delay=0;delay<4800000;delay++); //2 saniyelik bekleme
		    imlec(2,2);				//2. satır 2. sutuna yazdırıyoruz
		    Lcd_Yaz("Arda Cicin");
		}
		if(kosul==1){	 //Beyaz Led çalışıyor
			kosul2=1;
			GPIO_PORTE_DATA_R &= ~0b00000001;
			GPIO_PORTE_DATA_R |=  0b00000010;
			GPIO_PORTE_DATA_R &= ~0b00000100;
			GPIO_PORTE_DATA_R &= ~0b00001000;
			imlec(1,2);
			Lcd_Yaz("Arda Cicin");
			for(delay=0;delay<4800000;delay++);
			imlec(2,3);
			Lcd_Yaz("Kocaeli Uni");
		}
		if(kosul==2){	//Yeşil Led çalışıyor
			kosul2=2;
			GPIO_PORTE_DATA_R &= ~0b00000001;
			GPIO_PORTE_DATA_R &= ~0b00000010;
			GPIO_PORTE_DATA_R |= 0b00000100;
			GPIO_PORTE_DATA_R &= ~0b00001000;
			Lcd_Yaz("i");
		    for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("ni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz(" Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("i Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("li Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("eli Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("aeli Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("caeli Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("ocaeli Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			for(i=1;i<16;i++){		//Burada bütün karakterler artık ekranda olduğundan dolayı
			    imlec(1,i);			//kaydırma işlemlerine for ile devam ediyorum
		        Lcd_Yaz("Kocaeli Uni");
		        for(delay=0;delay<1000000;delay++);
		        Lcd_Temizle();
			}
			imlec(1,3);
			Lcd_Yaz("Kocaeli Uni");
			for(delay=0;delay<4800000;delay++);
			/*imlec(2,1);
			Lcd_Yaz("n");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("in");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("cin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("icin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz(" Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("a Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("da Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("rda Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("Arda Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();*/
			for(i=1;i<16;i++){
				imlec(1,3);
				Lcd_Yaz("Kocaeli Uni");
				imlec(2,i);
				Lcd_Yaz("Arda Ciçin");
				for(delay=0;delay<1000000;delay++);
				Lcd_Temizle();
			}
			imlec(1,3);
			Lcd_Yaz("Kocaeli Uni");
			imlec(2,2);
			Lcd_Yaz("Arda Cicin");
		}
		if(kosul==3){
			kosul2=3;
			GPIO_PORTE_DATA_R &= ~0b00000001;
			GPIO_PORTE_DATA_R &= ~0b00000010;
			GPIO_PORTE_DATA_R &= ~0b00000100;
			GPIO_PORTE_DATA_R |= 0b00001000;

			for(i=16;i>0;i--){
				imlec(1,i);
				Lcd_Yaz("Kocaeli Uni");
				for(delay=0;delay<1000000;delay++);
				Lcd_Temizle();
			}

			Lcd_Yaz("ocaeli Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("caeli Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("aeli Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("eli Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("li Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("i Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz(" Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("Uni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("ni");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			Lcd_Yaz("i");
		    for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(1,3);
			Lcd_Yaz("Kocaeli Uni");
			for(delay=0;delay<4800000;delay++);
			for(i=16;i>0;i--){
				imlec(1,3);
				Lcd_Yaz("Kocaeli Uni");
				imlec(2,i);
				Lcd_Yaz("Arda Cicin");
				for(delay=0;delay<1000000;delay++);
				Lcd_Temizle();
			}

			/*imlec(2,1);
			Lcd_Yaz("rda Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("da Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("a Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz(" Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("Cicin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("icin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("cin");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("in");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("n");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();
			imlec(2,1);
			Lcd_Yaz("");
			for(delay=0;delay<1000000;delay++);
			Lcd_Temizle();*/

			imlec(1,3);
			Lcd_Yaz("Kocaeli Uni");
			imlec(2,2);
			Lcd_Yaz("Arda Cicin");
		}
		for(delay=0;delay<7200000;delay++);		//Led yanıp işlemi bittikten sonra 3 saniye
		Lcd_Temizle();		//Ekranı temizler   //beklemesini sağlayan kod
		}
	}





  }
