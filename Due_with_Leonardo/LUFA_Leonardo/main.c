#include <avr/io.h>
#include <util/delay.h>

int main(void){
	DDRD|=(1<<PD5);

	while(1){
		_delay_ms(1000);
		PORTD^=(1<<PD5);

	}

}
