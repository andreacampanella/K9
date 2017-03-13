#define packet_header 0xAC //the start of the packet
#define DEBUG
#define M1A_PWM 12
#define M1A_Init 3
#define M1A_Break 9

#define M1B_PWM 13
#define M1B_Init 11
#define M1B_Break 8

int packet_lenght = 0   ;
int packet_data[10] = {0,0,0,0,0,0,0,0,0,0} ;
int packet_checksum = 0 ;

int inByte = 0 ; //a temporary variable to store our incoming byte


/*
  M_Init  : enable line of the motor
  M_Break : hold line of the motor 
  M_PWM   : PWM output pin
  M_SPEED : Speed (0-255) 
  M_DIR   : Direction (1: Forward 2: Backward)
*/
/*
	the communication packet will be something like this 
	
	0xAC 0x02 0xDE 0xAD 0x39

	0xAC is the header of out packet , this , with the checksum , will help the communication se-sync in case of error.
	0x01 is the leght of the payload (zero based, 1 is two bytes)
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
  Serial.begin(9600);
 // while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}
  
}

void loop() {
              
	/*
		Data collection phase:
		in this phase you will collect the data form the serial port , 
		check if is valid and do something useful with it .
	*/
	if (Serial.available() > 0) {
		// get incoming byte:
		inByte = Serial.read();
		if (inByte == packet_header)    	
		{
		  	#ifdef DEBUG
      	Serial.print("0xac ");      
      	#endif
      
			//first byte found. 
      while(!Serial.available()){}

			packet_lenght = Serial.read();

      if (packet_lenght > 0)    		
			{
          #ifdef DEBUG
          Serial.print(packet_lenght);
          #endif
				//if the lenght of the data is at least 1 bit long, we have valid data.
				//if not, well, why are you transmitting empty packets anyway???
				//start to collect as much bytes as leght says.
				for (int i = 0; i < packet_lenght; ++i)
				{
					packet_data[i] = Serial.read();
				}
				//let's read the checksum
				packet_checksum = Serial.read() ;
				
				int sum = packet_header+packet_lenght ;
   				
   				for (int i = 0; i < packet_lenght; i++) {
       				sum += packet_data[i];
   				}
          sum = sum & 0xFF;
   				
   				if ( sum = packet_checksum)
   				{
            #ifdef DEBUG
            //Serial.print("0x");
            //Serial.print(packet_header,HEX);
            Serial.print(" " );            
            Serial.print(packet_lenght);
            Serial.print(" " );
            Serial.print(packet_data[0]);
            Serial.print(" " );
            Serial.println(packet_data[1]);
            Serial.print(" " );
            //Serial.println(packet_data[2]);
            #endif

 					  Move(M1A_Init,M1A_Break,M1A_PWM,int(packet_data[0]), 1);
  					Move(M1B_Init,M1B_Break,M1B_PWM,int(packet_data[1]), 1);
   				}
          else{
            #ifdef DEBUG
            Serial.println("checksum error");
            #endif            
            
            }
    		}
	}
  }
}


