/*
  M_Init  : enable line of the motor
  M_Break : hold line of the motor 
  M_PWM   : PWM output pin
  M_SPEED : Speed (0-255) 
  M_DIR   : Direction (1: Forward 2: Backward)
*/
#define packet_header 0xAC //the start of the packet
#define DEBUG
#define M1A_PWM 12
#define M1A_Init 3
#define M1A_Break 9

#define M1B_PWM 13
#define M1B_Init 11
#define M1B_Break 8

int packet_data[10] = {0,0,0,0,0,0,0,0,0,0} ;

/*
	the communication packet will be something like this 
	
	0xAC 0x02 0xDE 0xAD 0x39

	0xAC is the header of out packet , this , with the checksum , will help the communication se-sync in case of error.
	0x02 is the leght of the payload 
	0xDE first byte of the payload
	0xAD second byte of the payload
	0x39 is the truckated (to 8 bit) checksum excluding header and checksum bytes
*/

void Move(int M_Init , int M_Break , int M_PWM, int M_SPEED, int M_DIR){    
  digitalWrite(M_Init, M_DIR); //Establishes forward direction of Channel A
  digitalWrite(M_Break, LOW);   //Disengage the Brake for Channel A
  analogWrite (M_PWM, M_SPEED);   //Spins the motor on Channel A at full speed
}

void Break(int M_Break){    
  digitalWrite(M_Break, HIGH);  //Engage the Brake
}



void setup() {
  //Setup Channel A
  pinMode(M1A_Init , OUTPUT); //Initiates Motor Channel A pin
  pinMode(M1A_Break, OUTPUT);  //Initiates Brake Channel A pin

  //Setup Channel B
  pinMode(M1B_Init , OUTPUT); //Initiates Motor Channel B pin
  pinMode(M1B_Break, OUTPUT);  //Initiates Brake Channel B pin

  // start serial port at 115200 bps:
  Serial.begin(115200);
  
  while (!Serial) {
  	 ; // wait for serial port to connect. Needed for native USB port only
  }
  
}

void loop() {
              
	/*
		Data collection phase:
		in this phase you will collect the data form the serial port , 
		check if is valid and do something useful with it .
	*/

	if (Serial.available() > 3) {
    /*
    The Arduino is not fast enought to handle a context switch between the serial port and the 
    code, we will have to collect the data in the serial port and analise it later.
    */
    delay(1); //let the UART collect all the packages
    int i = 0;

    while ((Serial.available() > 0) )
    {   
      packet_data[i] = Serial.read();
      i++;

    }
  
    int total_packet_len = packet_data[1]+2 ; //this is useful only to keep the code tidy.

    //check if the header is right.
		if (packet_data[0] == packet_header)   	
		{

      //Check if the lenght is valid.
      if (packet_data[1] > 0) 		
			{

				/*Let's read the checksum*/
				
				long sum = 0 ; 
        
        //Sum every element in the packet until you reach the checksum

   			for (i = 0; i < total_packet_len; i++) {
       			sum += packet_data[i];
   			}

        sum = sum & 0xFF ;
   				
   			if ( sum  == packet_data[total_packet_len] )
   			{
          #ifdef DEBUG
          Serial.print("<  " );
          for (int i = 0; i < packet_data[1]+3; ++i)
          {
            Serial.print("0x");
            Serial.print(packet_data[i],HEX);
            Serial.print(" ");
          }          
          Serial.println(" ");
          #endif

 				  Move(M1A_Init,M1A_Break,M1A_PWM,int(packet_data[2]), 1);
  				Move(M1B_Init,M1B_Break,M1B_PWM,int(packet_data[3]), 1);

   			}
        else{
          #ifdef DEBUG
          Serial.print("> ");
          Serial.print(packet_data[0],DEC);      
          Serial.print(" ");      
          Serial.print(packet_data[1],DEC);      
          Serial.print(" ");
          Serial.print(packet_data[2],DEC);
          Serial.print(" ");
          Serial.print(packet_data[3],DEC);
          Serial.print(" ");
          Serial.print(packet_data[4],HEX);
          Serial.print(" ");
          Serial.print("checksum error : ");
          Serial.print(sum , HEX);
          Serial.print(" Should be: ");
          Serial.print(packet_data[packet_data[1]+2] , HEX);         
          #endif                      
         }
    	}
    }
  }
}


