/*
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = { 
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
//IPAddress ip(192,168,2, 77);
//IPAddress gateway(192,168,2, 1);
IPAddress ip(192,168,0, 69);
IPAddress gateway(192,168,0, 1);
IPAddress subnet(255, 255, 255, 0);

// telnet defaults to port 23
EthernetServer server(23);
boolean gotAMessage = false; // whether or not you got a message from the client yet

void setup() {
  // Open serial communications and wait for port to open:
  //Serial.begin(9600);
  // this check is only needed on the Leonardo:
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for Leonardo only
  //}


  // start the Ethernet connection:
  //Serial.println("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    //Serial.println("Failed to configure Ethernet using DHCP");
    // initialize the ethernet device not using DHCP:
    Ethernet.begin(mac, ip, gateway, subnet);
  }
  // print your local IP address:
  //Serial.print("My IP address: ");
  //ip = Ethernet.localIP();
  //for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    //Serial.print(ip[thisByte], DEC);
    //Serial.print("."); 
  }
  //Serial.println("");
  
  //Start the TCP server and Keyboard functionality
  server.begin();
  Keyboard.begin();    
      
 
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();
  unsigned long time;
  
  // when the client sends the first byte, say hello:
  if (client) {
    String clientMsg ="";
    time = millis();
    
    //Get rid of all the handshaking garbage. We don't need no stupid handshaking.
    while(millis() - time <  1000) {
      client.read();
    }
    client.println("\n=============================================");
    client.println("Welcome to ardkey version 0.1");
    client.println("");
    client.println("The following special keys are available:");
    client.println("  CTL - Left control key");
    client.println("  ALT - Left control key");
    client.println("  SHT - Left shift key");
    client.println("  WIN - Left windows/command key");
    client.println("  UPP - Up arrow");
    client.println("  DWN - Down arrow");
    client.println("  LFT - Left arrow");
    client.println("  RIT - Right arrow");
    client.println("  BAS - Backspace");
    client.println("  DEL - Delete key");
    client.println("  ESC - Escape key");
    client.println("  RTN - Enter key");
    client.println("  TAB - Tab key");
    client.println("");
    client.println("The following commands are available:");
    client.println("  ONE - Hold down the following character");
    client.println("  TWO - Hold down the following two characters");
    client.println("  RLS - Release all keys");
    client.println("  SLP - Sleep for 3 seconds before sending subsequent key strokes");
    client.println("");
    client.println("See http://code.google.com/p/ardkey/wiki/DesignDescription for detailed examples.");
    client.println("=============================================");
    client.println();
    client.print("ArdKey> ");
    //int handled_connection = 0;
    
    while (client.connected()) {
      if (client.available()) {
        // read the bytes incoming from the client:
        char c = client.read();
       
        if ( (c >= 32 && c <= 126) || c == '\t' || c == '\n' ) {
          //if the character is an "end of line" the whole message is recieved
          if (c == '\n') {
            client.println("You said: "+clientMsg);//modify the string and send it back
            client.flush();
            
            //Replace any 3tuple with appropriate special characters (CTRL, ALT, SHIFT, WIN, BACKSPACE, UP, DOWN, LEFT, RIGHT)
            int length = clientMsg.length();
            int i = 0;
            String newClientMessage = "";
            while(i < length) {
              if (length - i >= 3) {
                String three_tuple = clientMsg.substring(i,i+3);
                if (three_tuple == "CTL") {
                  newClientMessage += (char) KEY_LEFT_CTRL;
                  i += 3;
                } else if (three_tuple == "ALT") {
                  newClientMessage += (char) KEY_LEFT_ALT;
                  i += 3;
                } else if (three_tuple == "SHT") {
                  newClientMessage += (char) KEY_LEFT_SHIFT;
                  i += 3;
                } else if (three_tuple == "WIN") {
                  newClientMessage += (char) KEY_LEFT_GUI;
                  i += 3;
                } else if (three_tuple == "UPP") {
                  newClientMessage += (char) KEY_UP_ARROW;
                  i += 3;
                } else if (three_tuple == "DWN") {
                  newClientMessage += (char) KEY_DOWN_ARROW;
                  i += 3;
                } else if (three_tuple == "LFT") {
                  newClientMessage += (char) KEY_LEFT_ARROW;
                  i += 3;
                } else if (three_tuple == "RIT") {
                  newClientMessage += (char) KEY_RIGHT_ARROW;
                  i += 3;
                } else if (three_tuple == "BAS") {
                  newClientMessage += (char) KEY_BACKSPACE;
                  i += 3;
                } else if (three_tuple == "DEL") {
                  newClientMessage += (char) KEY_DELETE;
                  i += 3;
                } else if (three_tuple == "ESC") {
                  newClientMessage += (char) KEY_ESC;
                  i += 3;
                } else if (three_tuple == "RTN") {
                  newClientMessage += (char) KEY_RETURN;
                  i += 3;
                } else if (three_tuple == "TAB") {
                  newClientMessage += (char) KEY_TAB;
                  i += 3;
                }
                else {
                  newClientMessage += clientMsg.substring(i,i+1);
                  i += 1;
                }
              } else {
                newClientMessage += clientMsg.substring(i,i+1);
                i += 1;
              }
            }
            clientMsg = newClientMessage;
            
            //Tell the user what their message got parsed to
            //client.println("We parsed that to: "+clientMsg);//modify the string and send it back
            //client.flush();
            
            //Process the string
            length = clientMsg.length();
            i = 0;
            while (i < length) {
              //Handle 3tuples
              if (length - i >= 3) {
                String three_tuple = clientMsg.substring(i,i+3);

                if (three_tuple == "ONE") {
                  i += 3;
                  if (length -i >= 1) {
                    client.println("Holding: " + clientMsg.substring(i,i+1) );
                    Keyboard.press(clientMsg.charAt(i));   
                    i += 1;
                  } else {
                      client.println("ERROR: Invalid syntax for ONE 3tuple. Expected one character after");
                  }
                } else if (three_tuple == "TWO") {
                  i += 3;
                  if (length -i >= 2) {
                    client.println("Holding: " + clientMsg.substring(i,i+1) + " and " + clientMsg.substring(i+1,i+2));
                    Keyboard.press(clientMsg.charAt(i));   
                    Keyboard.press(clientMsg.charAt(i+1));   
                    i += 2;
                  } else {
                      client.println("ERROR: Invalid syntax for TWO 3tuple. Expected two characters after");
                  }
                } else if (three_tuple == "RLS") {
                  i += 3;
                  client.println("Releasing All");
                  Keyboard.releaseAll(); 
                }  else if (three_tuple == "SLP") {
                  i += 3;
                  client.println("Sleeping for 3 seconds");
                  delay(3000);
                } else {
                  client.println("Tapping: " + clientMsg.substring(i,i+1) );
                  Keyboard.press(clientMsg.charAt(i));
                  Keyboard.release(clientMsg.charAt(i));
                  i += 1;
                }
              } else {
                client.println("Tapping: " + clientMsg.substring(i,i+1) );
                Keyboard.press(clientMsg.charAt(i));
                Keyboard.release(clientMsg.charAt(i));
                i += 1;
              }
              client.flush();
            }
            //Done processing, reset everything and wait for the next input
            clientMsg="";
            client.print("\nArdKey> ");
          } else {
            clientMsg+=c;//store the recieved chracters in a string
          }
        }
      }
    }
  }
}

