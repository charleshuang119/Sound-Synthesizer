//DEC 4 code

#include "./drivers/inc/vga.h"
#include "./drivers/inc/ISRs.h"
#include "./drivers/inc/LEDs.h"
#include "./drivers/inc/audio.h"
#include "./drivers/inc/HPS_TIM.h"
#include "./drivers/inc/int_setup.h"
#include "./drivers/inc/wavetable.h"
#include "./drivers/inc/pushbuttons.h"
#include "./drivers/inc/ps2_keyboard.h"
#include "./drivers/inc/HEX_displays.h"
#include "./drivers/inc/slider_switches.h"
#include "./drivers/inc/bmp.h"
int t=0;// global index allowing the soundwave to stay in phase
int soundOnly=0;// sound only option toggle
int enableSquareWave=0;// square wave toggle
int keyValue=-5;// changing key press detection
int buffFilled=0;// boolean if wave clearing buffer is filled
int clearBuffer[480];//wave form clearing buffer
double noteStatus[16]={0,0,0,0,0,0,0,0,130.813,146.832,164.814,174.614,195.998,220.000,246.942,261.626};//array composed of key press flags and frequencies of pressed keys
int currentAmplitude=80;// current volume goes from 80 to 160
//builds a sine wave with a given frequency, index and volume ratio, has the option to make square waves
int makeWave(double f,double t,int amp){
	double index=0;
	int samplingFreq=48000;
	index=(int)((f*t))%samplingFreq;
	int signal=sine[(int)index]*amp;
	if(enableSquareWave==1){
		signal =(index<24000)? 8388608*amp:-8388608*amp;
	} 
	return signal;
}

//adds waves of pressed keys and normalizes the resulting amplitude
int mergeWaves(int t){
	int i=0;
	int signal=0;
	int numActive=0;
	for(i=0;i<8;i++){
		if(noteStatus[i]==1){
			numActive++;
			signal+=makeWave(noteStatus[8+i],t,currentAmplitude);
		}
	}
	signal=signal/numActive;
	return (int)signal;
}
// converts HTML style RGB values to pixel buffer rgb values
short convRGB(int r,int g,int b){
	r=((double)r/255.0)*31;
	g=((double)g/255.0)*63;
	b=((double)b/255.0)*31;
	short rgb=(b)+(g<<5)+(r<<11);
	return rgb;
}
//displays a volume bar
void displayVolume(){
	int x=0;
	for (x=0;x<121;x++){
		if(currentAmplitude>=x/(1.2)){
			VGA_draw_point_ASM(120+x,220,convRGB(255,255,255));
			VGA_draw_point_ASM(120+x,221,convRGB(255,255,255));
			VGA_draw_point_ASM(120+x,222,convRGB(255,255,255));
			VGA_draw_point_ASM(120+x,223,convRGB(255,255,255));
			VGA_draw_point_ASM(120+x,224,convRGB(255,255,255));
			VGA_draw_point_ASM(120+x,225,convRGB(255,255,255));
			VGA_draw_point_ASM(120+x,226,convRGB(255,255,255));
			VGA_draw_point_ASM(120+x,227,convRGB(255,255,255));
		}else{
			VGA_draw_point_ASM(120+x,220,convRGB(100,100,100));
			VGA_draw_point_ASM(120+x,221,convRGB(100,100,100));
			VGA_draw_point_ASM(120+x,222,convRGB(100,100,100));
			VGA_draw_point_ASM(120+x,223,convRGB(100,100,100));
			VGA_draw_point_ASM(120+x,224,convRGB(100,100,100));
			VGA_draw_point_ASM(120+x,225,convRGB(100,100,100));
			VGA_draw_point_ASM(120+x,226,convRGB(100,100,100));
			VGA_draw_point_ASM(120+x,227,convRGB(100,100,100));
		}
		
	}
}

//go through every possible keypress and update the noteStatus array from 0 to 1 if key down also set activeKey to 1 if at least one key is down
//then check for break code then depending on the next read set back noteStatus form 1 to 0 meaning the key is released
int updateKeyStatus(){
	int activeKey=0;
	char data=0;
	read_ps2_data_ASM(&data);
	int foundBreak=0;
	if(data==0xF0){
		while(!foundBreak){//finding break codes
			read_ps2_data_ASM(&data);

			
			if(data==0x1C){//A
				noteStatus[0]=0;
				foundBreak=1;
				
			}
			else if(data==0x1B){//S
				noteStatus[1]=0;
				foundBreak=1;
			}
			else if(data==0x23){ //D
				noteStatus[2]=0;
				foundBreak=1;
			}
			else if(data==0x2B){ //F
				noteStatus[3]=0;
				foundBreak=1;
			}
			else if(data==0x3B){ //J
				noteStatus[4]=0;
				foundBreak=1;
			}
			else if(data==0x42){ //K
				noteStatus[5]=0;
				foundBreak=1;
			}
			else if(data==0x4B){ //L
				noteStatus[6]=0;
				foundBreak=1;
			}
			else if(data==0x4C){ //;
				noteStatus[7]=0;
				foundBreak=1;
			}else if(data==0x55){ //+
				foundBreak=1;
				currentAmplitude+=10;
				if (currentAmplitude>=100){
					currentAmplitude=100;
				}
				displayVolume();
			}
			else if (data==0x4E) {//-
				foundBreak=1;
				currentAmplitude-=10;
				if (currentAmplitude<1){
					currentAmplitude=0;
				}
				displayVolume();
			}else if (data==0x45) {//0   squarewave toggle
				foundBreak=1;
				enableSquareWave=1-enableSquareWave;
				VGA_draw_point_ASM(86,164,convRGB(0,204,255)*enableSquareWave);
				VGA_draw_point_ASM(87,164,convRGB(0,204,255)*enableSquareWave);
				VGA_draw_point_ASM(86,165,convRGB(0,204,255)*enableSquareWave);
				VGA_draw_point_ASM(87,165,convRGB(0,204,255)*enableSquareWave);
			}else if (data==0x46) {//9  only sound toggle
				foundBreak=1;
				soundOnly=1-soundOnly;

				VGA_draw_point_ASM(86,188,convRGB(0,204,255)*soundOnly);
				VGA_draw_point_ASM(87,188,convRGB(0,204,255)*soundOnly);
				VGA_draw_point_ASM(86,189,convRGB(0,204,255)*soundOnly);
				VGA_draw_point_ASM(87,189,convRGB(0,204,255)*soundOnly);


			}else foundBreak=0;

		}
	}
	else if(data==0x1C){//A
		noteStatus[0]=1;
		
	}
	else if(data==0x1B){//S
		noteStatus[1]=1;
		
	}
	else if(data==0x23){ //D
		noteStatus[2]=1;

	}
	else if(data==0x2B){ //F
		noteStatus[3]=1;
	}
	else if(data==0x3B){ //J
		noteStatus[4]=1;
	}
	else if(data==0x42){ //K
		noteStatus[5]=1;
	}
	else if(data==0x4B){ //L
		noteStatus[6]=1;
	}
	else if(data==0x4C){ //;
		noteStatus[7]=1;
	}
	int keysAreActive=0;
	int j=0;
	for (j=0;j<8;j++){
		keysAreActive+=noteStatus[j];
	}
	if(keysAreActive>0){
		return 1;
	}else {
		return 0;
	}

}
//edits black raw data pixels into our desired color which in this case is light blue
short pixelEdit(int r,int g,int b){
	if(r==0&&b==0&&g==255){
		r=0;
		b=187;
		g=234;
	}
	r=((double)r/255.0)*31;
	g=((double)g/255.0)*63;
	b=((double)b/255.0)*31;
	short rgb=(b)+(g<<5)+(r<<11);
	return rgb;
}

//displays pressed key, piano style
void displayKeys(){
	int a=0;
	for(a=0;a<8;a++){
		int x=0;
		int y=0;
		for(x=0;x<8;x++){
			for(y=0;y<30;y++){
				VGA_draw_point_ASM(110+x+a*20,160+y,convRGB(100,100,100)*(int)noteStatus[a]+convRGB(255,255,255)*(int)(1-noteStatus[a]));
			}
		}
	}
}

//prases given raw image data into pixelbuffer pixel format and draws them accordingly, the raw image data in this case is a mario image
void drawBMP(){
	short rgb=0;
	int con[19200];
	int x=0,a=0,b=0,c=0,r=0,g=0;
	for( x=0;x<9600;x++){//sparates raw data into individual RGB values and stores them into the con[] array
		 a=bmp[x];
		 b=a>>8;
		 c=a-(b<<8);
		 con[x*2]=b;
		 con[x*2+1]=c;
	}
	for(x=0;x<6400;x++){//read from the //con array to get specific pixel colors and draw them
		r=con[x*3];
		g=con[x*3+1];
		b=con[x*3+2];
		rgb=pixelEdit(r,g,b);
		VGA_draw_point_ASM(x%80,x/80,rgb);
	}
	

}
//prases given raw image data into pixelbuffer pixel format and draws them accordingly, the raw image data in this case is the UI
void drawUIBMP(){
	short rgb=0;
	int con[230400];
	int x=0,a=0,b=0,c=0,r=0,g=0;
	for( x=0;x<115200;x++){
		 a=ui[x];
		 b=a>>8;
		 c=a-(b<<8);
		 con[x*2]=b;
		 con[x*2+1]=c;
	}
	for(x=0;x<76800;x++){
		r=con[x*3];
		g=con[x*3+1];
		b=con[x*3+2];
		rgb=pixelEdit(r,g,b);
		VGA_draw_point_ASM(x%320,x/320,rgb);
	}
	

}

//main part of software
//loops continously while checking for key presses, calling wave drawing and key drawing functions,
//a sample is played by going through a loop that plays one sample, we chose a counter of 480
//this period will allows us to also draw at least one period of any note
//a pariod of 480 means we will be able to update the key press status 48000/480=100 time a second
void playWave(){
	int c=0;
	int samplingFreq=48000;
	int signal=0;
	int lowestFreq=100;
	int period=samplingFreq/lowestFreq;
	//initializing the timer
	HPS_TIM_config_t hps_tim;
	hps_tim.tim = TIM0;
	hps_tim.timeout =1000000/48000; 
	hps_tim.LD_en=1;
	hps_tim.INT_en=0;
	hps_tim.enable=1;
	HPS_TIM_config_ASM(&hps_tim);
	int temp=0;
	int scaledSignal=0;
	int scaledX=0;
	
	while(1){
		//this draws a straight line when no sound is being played
		for(c=0;c<period&&soundOnly==0;c++){
			scaledX=100+(c/479.0*200.0);
			if(buffFilled==1){
				VGA_draw_point_ASM(scaledX,clearBuffer[c],convRGB(200,35,217));
			}
				clearBuffer[c]=100;

				VGA_draw_point_ASM(scaledX,100,convRGB(0,204,255));
				
		}

		while(updateKeyStatus()){
		//here we check if the keyspressed were modified in order to avoid doing costly oprations which are not needed
		int currentKeyValue=noteStatus[0]*3+noteStatus[1]*5+noteStatus[2]*7+noteStatus[3]*11+noteStatus[4]*13+noteStatus[5]*17+noteStatus[6]*19+noteStatus[7]*23;
		if(currentKeyValue!=keyValue&&soundOnly==0){displayKeys();}
		keyValue=currentKeyValue;	
		if(HPS_TIM_read_INT_ASM(TIM0)){
			HPS_TIM_clear_INT_ASM(TIM0);
			c=0;
			while(c<period){

				signal=mergeWaves(t);//get the merged signal


				temp=t%6000;//this allows us to update the wave form 8 times per second

				if(temp>=0&&temp<480&&soundOnly==0){// this will draw a one wave sample every 1/8th of a second
					if(temp==479){
						buffFilled=1;
					}
					scaledSignal=100+(signal/60.0)/(8388608.0)*30.0;//the signal is scaled down for i to be drawn
					scaledX=100+(temp/479.0*200.0);//the signal is scaled down for i to be drawn
					if(buffFilled==1){//this allows us to only clear the drawn waveform pixels instead of the whole screen which saves a big amount of time
					VGA_draw_point_ASM(scaledX,clearBuffer[temp],convRGB(200,35,217));
					}
					clearBuffer[temp]=scaledSignal;

				VGA_draw_point_ASM(scaledX,scaledSignal,convRGB(0,204,255));
				}

				if(audio_write_data_ASM(signal,signal)){//we finaly write the audio data to the FIFOs
					t++;
					c++;
				}		
			}							
		}
	}
	}
}

//this clears all buffers, draws the UI, which consists of the raw data ui background, the piano keys,the volume bar,
//the piano keys names and finaly the mario raw data
void drawUI(){
	VGA_clear_charbuff_ASM();
	VGA_clear_pixelbuff_ASM();
	drawUIBMP();
	displayKeys();
	displayVolume();
	VGA_write_char_ASM(28,48,'C');
	VGA_write_char_ASM(33,48,'D');
	VGA_write_char_ASM(38,48,'E');
	VGA_write_char_ASM(43,48,'F');
	VGA_write_char_ASM(48,48,'G');
	VGA_write_char_ASM(53,48,'A');
	VGA_write_char_ASM(58,48,'B');
	VGA_write_char_ASM(63,48,'C');
	drawBMP();
}

int main() {

	drawUI();
	playWave();
}
