import oscP5.*;
import netP5.*;

OscP5 oscP5;
NetAddress myRemoteLocation;


void setup()
{
  size(400, 400);

  // start oscP5, telling it to listen for incoming messages at port 9000 */
  oscP5 = new OscP5(this, 9000);
}

void oscEvent(OscMessage message) 
{
  if (message.addrPattern().equals("/wii/1/balance")) {
    float bottomLeft = message.get(0).floatValue();
    float bottomRight = message.get(1).floatValue();
    float topLeft = message.get(2).floatValue();
    float topRight = message.get(3).floatValue();
    float sum = message.get(4).floatValue();
    float virtualX = message.get(5).floatValue();
    float virtualY = message.get(6).floatValue();
    // print out the message
    print("Wii Board OSC Message Received: ");
    print(message.addrPattern() + " ");
    println("X: " + virtualX + ", Y: " + virtualY);
  }
}

void draw()
{
 
}
